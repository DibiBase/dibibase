#pragma once

#include "commitlog/commitlog.hh"

// A set of global variables to be replaced with a proper config file
// TODO: get these values from the config.yml file
const char *DEFAULT_COMMITLOG_LOCATION = "../logs/";
int32_t COMMITLOG_MAX_SIZE_IN_MEGABYTES = 10;

commitlog::Commitlog::Commitlog(const char *filename) : max_size(COMMITLOG_MAX_SIZE_IN_MEGABYTES * 1024) {

  std::string fullpath(DEFAULT_COMMITLOG_LOCATION);
  fullpath.append(filename);
  log_file_name = fullpath.c_str();
  log_file_descriptor = open(fullpath.c_str(), O_RDWR | O_CREAT | O_EXCL, 0755);
  if ((log_file_descriptor == -1) && (ENOENT == errno)) {
    // directory not found, create the directory and try again
    mkdir(DEFAULT_COMMITLOG_LOCATION, 0775);
    log_file_descriptor = open(fullpath.c_str(), O_RDWR | O_CREAT | O_EXCL, 0755);
  }

  if ((log_file_descriptor == -1) && (EEXIST == errno)) {
    // The file is already there, instead of creating it, we'll read its header
    log_file_descriptor = open(fullpath.c_str(), O_RDWR, 0755);
    read_file_header();
  }
  else{
    // The file is created for the first time
    append_position = 0;
    archived = false;
    active = true;
    size = commitlog::Commitlog::COMMITLOG_HEADER_SIZE; //33
    version = commitlog::Commitlog::CURRENT_VERSION; // 0
    construct_file_header();
}
}


commitlog::Commitlog::Commitlog() : Commitlog(DEFAULT_COMMITLOG_LOCATION) {};

commitlog::Commitlog::~Commitlog() {
  close(log_file_descriptor);
}

void commitlog::Commitlog::flush(std::unique_ptr<char[]> buffer, int32_t buffer_size) {
  ssize_t bytes_written;
  lseek(log_file_descriptor, 0, SEEK_END);
  size += buffer_size;
  bytes_written = write(log_file_descriptor, buffer.get(), buffer_size);
  size += bytes_written;
}

void commitlog::Commitlog::flush(std::unique_ptr<unsigned char[]> buffer, int32_t buffer_size) {
  ssize_t bytes_written;
  lseek(log_file_descriptor, 0, SEEK_END);
  size += buffer_size;
  bytes_written = write(log_file_descriptor, buffer.get(), buffer_size);
  size += bytes_written;
}

bool commitlog::Commitlog::construct_file_header() {
  /**
   * Commitlog file header (33 bytes)
   * ---------------------------------------------------------------------------------------------------
   * | DIBIBASE_LOG_FILE | version  | ID       |  is_archived | is_active  | Size    | append_position |
   * | 17 bytes          | 2 bytes  | 4 bytes  |    1 byte    | 1 byte     | 4 bytes | 4 bytes         |
   * ---------------------------------------------------------------------------------------------------
   */
  std::unique_ptr<char[]> buffer(new char[commitlog::Commitlog::COMMITLOG_HEADER_SIZE]);
  memcpy(buffer.get(), "DIBIBASE_LOG_FILE",
         17); // used to identify commit log files from other files in the same directory
  int offset = 17;
  append_position = commitlog::Commitlog::COMMITLOG_HEADER_SIZE; // append position for records, starts after buffer
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&version), sizeof(int16_t));
  offset += sizeof(int16_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&id), sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&archived), sizeof(bool));
  offset += sizeof(bool);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&active), sizeof(bool));
  offset += sizeof(bool);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&size), sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&append_position), sizeof(int32_t));

  // writing the buffer to the log file
  lseek(log_file_descriptor, 0, SEEK_SET);
  size_t bytes_written;
  bytes_written = write(log_file_descriptor, buffer.get(), commitlog::Commitlog::COMMITLOG_HEADER_SIZE);

  if (bytes_written == commitlog::Commitlog::COMMITLOG_HEADER_SIZE) {
    return true;
  }
  return false;
}

bool commitlog::Commitlog::read_file_header() {
  std::unique_ptr<char[]> buffer(new char[commitlog::Commitlog::COMMITLOG_HEADER_SIZE + 1]);
  size_t offset = 0;
  lseek(log_file_descriptor, 0, SEEK_SET);
  offset = read(log_file_descriptor, buffer.get(), commitlog::Commitlog::COMMITLOG_HEADER_SIZE);
  // verifying that the open file is a valid dibibase log file
  std::string data;
  for (int i = 0; i < 17; ++i) {
    data.push_back((char) buffer[i]);
  }
  if (data != "DIBIBASE_LOG_FILE") {
    // file is in the logs directory but not a log file
    return false;
  }
  // reconstructing log parameters from file header
  memcpy(reinterpret_cast<unsigned char *>(&version), buffer.get() + commitlog::Commitlog::VERSION_OFFSET, sizeof(int16_t));
  memcpy(reinterpret_cast<unsigned char *>(&id), buffer.get() + commitlog::Commitlog::ID_OFFSET, sizeof(int32_t));
  memcpy(reinterpret_cast<unsigned char *>(&archived), buffer.get() + commitlog::Commitlog::ARCHIVED_STATUS_OFFSET, sizeof(bool));
  memcpy(reinterpret_cast<unsigned char *>(&active), buffer.get() + commitlog::Commitlog::ACTIVE_STATUS_OFFSET, sizeof(bool));
  memcpy(reinterpret_cast<unsigned char *>(&size), buffer.get() + commitlog::Commitlog::SIZE_OFFSET, sizeof(int32_t));
  memcpy(reinterpret_cast<unsigned char *>(&append_position), buffer.get() + commitlog::Commitlog::APPEND_POSITION_OFFSET,
         sizeof(int32_t));
  return true;
}

void commitlog::Commitlog::update_active_status(bool status) {
  active = status;
  std::unique_ptr<char[]> buffer(new char[sizeof(bool)]);
  memcpy(buffer.get(), reinterpret_cast<unsigned char *>(&active), sizeof(bool));
  lseek(log_file_descriptor, commitlog::Commitlog::ACTIVE_STATUS_OFFSET, SEEK_SET);
  write(log_file_descriptor, buffer.get(), sizeof(bool));
}

void commitlog::Commitlog::update_archived_status(bool status) {
  archived = status;
  std::unique_ptr<char[]> buffer(new char[sizeof(bool)]);
  memcpy(buffer.get(), reinterpret_cast<unsigned char *>(&archived), sizeof(bool));
  lseek(log_file_descriptor, commitlog::Commitlog::ARCHIVED_STATUS_OFFSET, SEEK_SET);
  write(log_file_descriptor, buffer.get(), sizeof(bool));
}

void commitlog::Commitlog::update_size(int32_t new_size) {
  size = new_size;
  std::unique_ptr<char[]> buffer(new char[sizeof(int32_t)]);
  memcpy(buffer.get(), reinterpret_cast<unsigned char *>(&size), sizeof(int32_t));
  lseek(log_file_descriptor, commitlog::Commitlog::SIZE_OFFSET, SEEK_SET);
  write(log_file_descriptor, buffer.get(), sizeof(int32_t));
}

void commitlog::Commitlog::update_append_position(int32_t offset) {
  append_position = offset;
  std::unique_ptr<char[]> buffer(new char[sizeof(int32_t)]);
  memcpy(buffer.get(), reinterpret_cast<unsigned char *>(&append_position), sizeof(int32_t));
  lseek(log_file_descriptor, commitlog::Commitlog::APPEND_POSITION_OFFSET, SEEK_SET);
  write(log_file_descriptor, buffer.get(), sizeof(int32_t));
}


int32_t commitlog::Commitlog::get_last_lsn() {
  int32_t record_size, record_lsn;
  int32_t largest_lsn = 0;

  std::unique_ptr<char[]> buffer(new char[2000]);

  int32_t read_offset = 1; // dummy value, doesn't matter unless it's 0, used to determine reaching EoF
  int32_t seek_offset = commitlog::Commitlog::COMMITLOG_HEADER_SIZE;// The first position after the file header
  while (read_offset != 0) { // read returns 0 if we reached EOF
    
    // 12 bytes are read as we want to read the size and lsn of the record
    // first 3 fields of record header: size(4 bytes)|type(4 bytes)|lsn(4 bytes)|
    lseek(log_file_descriptor, seek_offset, SEEK_SET);
    read_offset = read(log_file_descriptor, buffer.get(), 12);
    if (read_offset == -1) {
      // read returned an error
      break;
      // TODO: log the errno
    }


    // getting the size of the record
    memcpy(reinterpret_cast<unsigned char *>(&record_size), buffer.get(), sizeof(int32_t));
    // read the lsn of the record
    memcpy(reinterpret_cast<unsigned char *>(&record_lsn), buffer.get() + 8, sizeof(int32_t));

    if (record_size <= 4 ){
      // an invalid record
      break;
    }

    // compare to the current largest lsn
    if (record_lsn > largest_lsn)
      largest_lsn = record_lsn;

    seek_offset += record_size;
  }

  // this is a workaround to finding the file size, since we'll be traversing each file once on start-up anyway
  size = seek_offset - record_size;
  return largest_lsn;
}

