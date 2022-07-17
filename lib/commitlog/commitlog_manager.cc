#include "commitlog/commitlog_manager.hh"

/**
 * A set of global variable to act as config
 * TODO: change this into a proper config.yml file
 */
std::string DEFAULT_COMMITLOG_DIRECTORY = "logs";
std::chrono::duration<long long int, std::milli> LOG_TIMEOUT = std::chrono::milliseconds(1000);

bool commitlog::CommitlogManager::ENABLE_LOGGING = true;

commitlog::CommitlogManager::CommitlogManager(SyncMode mode)
    : commitlog(nullptr), sync_mode{mode}, need_flush(false),
      lsn(0), persistent_lsn(0), flush_thread(nullptr),
      default_directory("../" + DEFAULT_COMMITLOG_DIRECTORY + "/"), db(nullptr) {
  // scanning all the files in the given directory TODO: get the directory from the yml config file
  DIR *d;
  struct dirent *dir;
  d = opendir(default_directory.c_str());
  if (d) {
    while ((dir = readdir(d)) != nullptr) {
      if (dir->d_type == DT_REG) { // filtering the regular files from directories or system/hidden files
        std::string file = std::string(dir->d_name);
        // checking that a file has Dibibase log files naming conventions
        if (file.substr(0, 18) == "dibibase_log_file_") {
          std::unique_ptr<Commitlog> temp_log = std::make_unique<Commitlog>(file.c_str());
          // whether the file is archived or not, we'd need to know the last lsn it holds
          int32_t file_largest_lsn = temp_log->get_last_lsn();
          if (file_largest_lsn > last_lsn) {
            last_lsn = file_largest_lsn;
          }
          // We append to a file if it's active and Not archived
          if (!temp_log->is_archived() && temp_log->is_active()) {
            commitlog = move(temp_log);

          } else if (!temp_log->is_archived() && !temp_log->is_active()) {
            // file is not active and not-archived (we can't append to it, but it's still
            // not persistent in DB, so we have to keep it
            logs_to_flush.push(move(temp_log));
          } else {
            // file is marked as archived. It was written to the DB and is no longer needed
            logs_to_delete.push(move(temp_log));
            //TODO: handle it (delete it? need a way to make sure the DB no longer needs it)
          }
        }
      }
    }
    closedir(d);
  }

  lsn.store(++last_lsn); // value would be 1 in case of no files found
  if (commitlog == nullptr) { // No active log files were found, so we create a new one
    commitlog = create_new_log_file();
  }
  if (sync_mode == SyncMode::BATCHED){
    run_flush_thread();
    need_flush.store(true);}
}

commitlog::CommitlogManager::~CommitlogManager() {
  if (log_buffer_offset != 0) {
    // log buffer is not empty, so we need to flush it
    force_flush();
  }
  // stop flush thread
  if (flush_thread != nullptr) {
    flush_thread->join();
  }
}

int32_t commitlog::CommitlogManager::append_log_record(std::string table_name, dibibase::util::Buffer *buf, int32_t buf_size) {

  std::unique_lock<std::mutex> latch(mutex_latch);

  int32_t header_size = 24 + table_name.size(); // 24 is the size of 6 int32_t fields
  int32_t total_size = header_size + buf->offset();  // size of header + data
  dibibase::util::MemoryBuffer header_buf(header_size);

  header_buf.put_int32(total_size);
  header_buf.put_int32(1); //LogRecordType, 1 is used for inserts & updates
  header_buf.put_int32(lsn);
  header_buf.put_int32(0); //previous lsn is added for undoing transactions which we don't support yet
  header_buf.put_int32(table_name.size());
  header_buf.put_string(table_name);
  header_buf.put_int32(buf->offset()); // length of the record's data

  
  std::unique_ptr<unsigned char[]> header(new unsigned char[total_size]);

  // copying the header to the log buffer
  memcpy(header.get(), header_buf.bytes().get(), header_size);
  // copying the data to the log buffer
  memcpy(header.get()+header_size, buf->bytes().get(), buf->offset()); 

  if (sync_mode == SyncMode::SYNC) {
    commitlog->flush(move(header), total_size);
  } 
  else if (sync_mode == SyncMode::BATCHED) {
    if (log_buffer_offset + header_size >= CommitlogManager::LOG_BUFFER_SIZE) {
      // not enough space in buffer, hence we have to flush first
      run_flush_thread();
      need_flush = true;
      flush_cv.notify_one();
      append_cv.wait(latch, [&] {
        return log_buffer_offset == 0;
      });
    }
    memcpy(log_buffer.get() + log_buffer_offset, header.get(), total_size);
  }
  log_buffer_offset += total_size;
  last_lsn = lsn;
  lsn.store(++lsn);
  // check if we surpassed the max_size
  if (commitlog->get_current_size() >= commitlog->get_max_size()) {
    commitlog->update_active_status(false);
    logs_to_flush.push(move(commitlog));
    commitlog.reset(); // not sure if this is redundant after moving the pointer ownership
    commitlog = create_new_log_file();
    
  }
  return --lsn;
}

void commitlog::CommitlogManager::run_flush_thread() {
    flush_buffer = std::make_unique<char[]>(CommitlogManager::LOG_BUFFER_SIZE);
    flush_thread = new std::thread([&] {
      std::unique_lock<std::mutex> latch(mutex_latch);
      flush_cv.wait_for(latch, LOG_TIMEOUT, [&] { return need_flush.load(); });
      //TODO: check if the flush buffer is not empty and handle that case, rn we overwrite its contents
      flush_buffer_size = 0;
      if (log_buffer_offset > 0) {
        std::swap(log_buffer, flush_buffer);
        std::swap(log_buffer_offset, flush_buffer_size);
        commitlog->flush(move(flush_buffer), flush_buffer_size);
        flush_buffer_size = 0;
        set_persistent_lsn(last_lsn);
      }
      need_flush = false;
      append_cv.notify_all();
    });
}


void commitlog::CommitlogManager::force_flush() {
  if (sync_mode == SyncMode::BATCHED) {
  std::unique_lock<std::mutex> latch(mutex_latch);
  run_flush_thread();
  need_flush = true;
  flush_cv.notify_one(); //let flush thread wake up.
  append_cv.wait(latch, [&] { return !need_flush.load(); }); //block append thread
  }
  if (sync_mode == SyncMode::SYNC) {
    // No threads required since we will block the commit until the disk write is done anyway.
    commitlog->flush(move(log_buffer), log_buffer_offset);
  }
}

std::unique_ptr<commitlog::Commitlog> commitlog::CommitlogManager::create_new_log_file() {
  if (commitlog != nullptr) {
    return move(commitlog);
  } else {
    // Naming the log files
    const char *prefix = "dibibase_log_file_";
    const char *suffix = ".log";
    std::string fullname(prefix + std::to_string(lsn) + suffix);

    std::unique_ptr<Commitlog> log = std::make_unique<Commitlog>(fullname.c_str());
    return move(log);
  }
}

void commitlog::CommitlogManager::apply_logs_queue(std::queue<std::unique_ptr<Commitlog>> &log_queue) {
  // making sure no records are still in memory
  force_flush();
  // marking the current log file as inactive so that it joins the recovery queue, and start a new log file
  commitlog->update_active_status(false);
  commitlog.reset();
  commitlog = create_new_log_file();

  while (!log_queue.empty()) {
    std::unique_ptr<Commitlog> log = move(log_queue.front());

    int fd = log->get_file_descriptor();
    lseek(fd, Commitlog::COMMITLOG_HEADER_SIZE, SEEK_SET); // The first position after the file header

    int32_t size;
    size_t offset = 1; // dummy value, doesn't matter unless it's 0
    while (offset != 0) { // read returns 0 if we reached EOF

      std::unique_ptr<unsigned char[]> buffer(new unsigned char[2000]);
      offset = read(fd, buffer.get(), 4);

      if (offset == -1) {
        // read returned an error
        break;
        // TODO: log the error
      }
      // getting the size of the record
      memcpy(reinterpret_cast<unsigned char *>(&size), buffer.get(), sizeof(int32_t));
      //read the entire record
      if (size < 4)
        break; //TODO: log the error as invalid record

      offset = read(fd, buffer.get(), size - 4);

      // create a record from the buffer
      std::unique_ptr<char[]> temp_buffer(new char[size-4]);
      dibibase::util::MemoryBuffer mem_buffer(move(buffer), size-4);
      recover_record(&mem_buffer, size-4);

    }
    mark_archived(move(log)); // archived log records are no longer needed
    logs_to_delete.push(move(log));
    log_queue.pop();



  }
}

void commitlog::CommitlogManager::apply_all_logs() {
  // logs to flush queue contains all unarchived log records
  apply_logs_queue(logs_to_flush);
}

void commitlog::CommitlogManager::apply_logs_from_lsn(int32_t recovery_lsn) {

  // queue of logs to be recovered
  std::queue<std::unique_ptr<Commitlog>> log_queue;

  // iterating over files in a log directory
  DIR *d;
  struct dirent *dir;
  d = opendir(default_directory.c_str());
  if (d) {
    while ((dir = readdir(d)) != nullptr) {
      if (dir->d_type == DT_REG) { // filtering the regular files from directories or system/hidden files
        std::string file = std::string(dir->d_name);

        if (file.substr(0, 18) ==
            "dibibase_log_file_") { // checking that a file has Dibibase log files naming conventions
          int file_starting_lsn = std::stoi(
              file.substr(18, file.size() - 4)); // get the starting lsn of a log file
          if (file_starting_lsn >=
              recovery_lsn) { // only files larger than recovery lsn should be added to the queue
            std::unique_ptr<Commitlog> log = std::make_unique<Commitlog>(file.c_str());
            log_queue.push(move(log));
          }
        }
      }
    }
    closedir(d);
  }
  apply_logs_queue(log_queue);

}

void commitlog::CommitlogManager::recover_record(dibibase::util::Buffer *buf, int32_t buf_size){
  // disable logging globaly
  commitlog::CommitlogManager::ENABLE_LOGGING = false;

  // read the buffer data
  int32_t type = buf->get_int32();
  int32_t lsn = buf->get_int32();
  int32_t previous_lsn = buf->get_int32();
  int32_t table_name_size = buf->get_int32();
  std::string table_name = buf->get_string(table_name_size);
  int32_t buffer_size = buf->get_int32();

  // this should be written to the database event log
  std::cout << "Recovering record with LSN: " << lsn << " in table: " << table_name<< std::endl;

  std::unique_ptr<unsigned char[]> record_data_buffer(new unsigned char[buffer_size]);
  memcpy(record_data_buffer.get(), buf->bytes().get()+20+table_name_size, buffer_size);

  dibibase::util::MemoryBuffer mem_buffer(move(record_data_buffer), buffer_size);
  catalog::Field id_field("id", catalog::Data::Type(catalog::Data::Type::Id::INT, 4));
  catalog::Field employee_name_field("employee_name", catalog::Data::Type(catalog::Data::Type::ASCII,8));

  // get table schema by table name
  catalog::Schema *schema = db->get_table_schema(table_name).get();

  // re-establish the record
  std::unique_ptr<dibibase::catalog::Record> record = dibibase::catalog::Record::from(&mem_buffer, *schema);

  db->write_record(table_name, *record.get());

  // enable logging
  commitlog::CommitlogManager::ENABLE_LOGGING = true;
}
