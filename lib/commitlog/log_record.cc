
#include <utility>

#include "commitlog/log_record.hh"


// Constructor for INSERT type log records
commitlog::LogRecord::LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
                     int32_t data_size, std::string data)
    : previous_lsn(prev_lsn), table_name_length(tableID), entry_size(data_size),
      entry_data(std::move(data)), lsn(0), tuple_id(0) {

  if (log_record_type == LogRecordType::INSERT) {
    record_type = log_record_type;
    // size of the entire log record
    size = sizeof(LogRecordType) + sizeof(int32_t) * 5 + entry_size;

  } // TODO: else write to logger Invalid log record
}


// Constructor for UPDATE type log records
// TODO: change data type to buffer
commitlog::LogRecord::LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
                     const int32_t tupleID, int32_t data_size, std::string data)
    : previous_lsn(prev_lsn), table_name_length(tableID), tuple_id(tupleID), entry_size(data_size),
      entry_data(std::move(data)) {

  if (log_record_type == LogRecordType::UPDATE) {
    record_type = log_record_type;
    // size of the entire log record
    size = sizeof(LogRecordType) + sizeof(int32_t) * 6 + entry_size;
    // LSN is the offset in the log file at which the record start

  } // TODO: else write to logger Invalid log record
}

// Constructor for DELETE type log records
commitlog::LogRecord::LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
                     const int32_t tupleID)
    : previous_lsn(prev_lsn), table_name_length(tableID), tuple_id(tupleID) {
  if (log_record_type == LogRecordType::DELETE) {
    record_type = log_record_type;
    // size of the entire log record
    size = sizeof(LogRecordType) + sizeof(int32_t) * 5;
  } // TODO: else write to logger Invalid log record

}

std::unique_ptr<char[]> commitlog::LogRecord::construct_record_buffer() {
  // Header is constant in all record types
  // | Log Size | Log Type | LSN | Previous LSN | TableName Size  |
  std::unique_ptr<char[]> buffer(new char[2000]);
  int offset = 0;
  memcpy(buffer.get(), reinterpret_cast<unsigned char *>(&size), sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&record_type), sizeof(LogRecordType));
  offset += sizeof(LogRecordType);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&lsn), sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&previous_lsn), sizeof(int32_t));
  offset += sizeof(int32_t);
  memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&table_name_length), sizeof(int32_t));
  offset += sizeof(int32_t);

  // handling the body of the record based on the record type
  if (record_type == LogRecordType::INSERT) {
    // body: | Entry Size | Entry Data |
    // writing entry size
    memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&entry_size), sizeof(int32_t));
    offset += sizeof(int32_t);
    // entry data
    memcpy(buffer.get() + offset, (entry_data.c_str()), entry_size);

  } else if (record_type == LogRecordType::UPDATE) {
    // Body: | TupleID | Entry Size | Entry Data |
    //TupleID
    memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&tuple_id), sizeof(int32_t));
    offset += sizeof(int32_t);
    // entry size
    memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&entry_size), sizeof(int32_t));
    offset += sizeof(int32_t);

    // entry data
    memcpy(buffer.get() + offset, entry_data.c_str(), entry_size);
  } else if (record_type == LogRecordType::DELETE) {
    // Body: | TupleID |
    //TupleID
    memcpy(buffer.get() + offset, reinterpret_cast<unsigned char *>(&tuple_id), sizeof(int32_t));
  }
  return std::move(buffer);
}

commitlog::LogRecord::LogRecord(std::unique_ptr<char[]> buffer) {
  // getting the type of the record
  int32_t type; // 1 for INSERT. 2 for UPDATE, 3 for DELETE
  memcpy(reinterpret_cast<unsigned char *>(&type), buffer.get(), 4);

  if (type == 1) {
    // INSERT log record
    memcpy(reinterpret_cast<unsigned char *>(&lsn), buffer.get() + 4, 4);
    memcpy(reinterpret_cast<unsigned char *>(&previous_lsn), buffer.get() + 8, 4);
    memcpy(reinterpret_cast<unsigned char *>(&table_name_length), buffer.get() + 12, 4);
    memcpy(reinterpret_cast<unsigned char *>(&entry_size), buffer.get() + 16, 4);
    record_type = LogRecordType::INSERT;
    // reading the data
    for (int i = 20; i < (int) (20 + entry_size); ++i) {
      entry_data.push_back((char) buffer[i]);
    }

  } else if (type == 2) {
    // UPDATE log record
    memcpy(reinterpret_cast<unsigned char *>(&lsn), buffer.get() + 4, 4);
    memcpy(reinterpret_cast<unsigned char *>(&previous_lsn), buffer.get() + 8, 4);
    memcpy(reinterpret_cast<unsigned char *>(&table_name_length), buffer.get() + 12, 4);
    memcpy(reinterpret_cast<unsigned char *>(&tuple_id), buffer.get() + 16, 4);
    memcpy(reinterpret_cast<unsigned char *>(&entry_size), buffer.get() + 20, 4);
    record_type = LogRecordType::UPDATE;
    // reading the data
    for (int i = 24; i < (int) (24 + entry_size); ++i) {
      entry_data.push_back((char) buffer[i]);

    }
  } else if (type == 3) {
    // DELETE log record
    memcpy(reinterpret_cast<unsigned char *>(&lsn), buffer.get() + 4, 4);
    memcpy(reinterpret_cast<unsigned char *>(&previous_lsn), buffer.get() + 8, 4);
    memcpy(reinterpret_cast<unsigned char *>(&table_name_length), buffer.get() + 12, 4);
    memcpy(reinterpret_cast<unsigned char *>(&tuple_id), buffer.get() + 16, 4);
    record_type = LogRecordType::DELETE;
  }
}

commitlog::LogRecord::LogRecord(dibibase::util::Buffer *buf){
  int type = buf->get_int32();
  if (type == 1) {
    // INSERT log record

  } else if (type == 2) {
    // UPDATE log record
    lsn = buf->get_int32();
    previous_lsn = buf->get_int32();
    table_name_length = buf->get_int32();
    table_name = buf->get_string(table_name_length);
    entry_size = buf->get_int32();
    entry_data = buf->get_string(entry_size);


  } else if (type == 3) {
    // DELETE log record

  }
  
}
static commitlog::LogRecord from_buffer(dibibase::util::Buffer &buffer){
  // commitlog::LogRecord record;
  // record.set_lsn(buffer.get_int32());
  // record.set_previous_lsn(buffer.get_int32());
  // record.set_table_name_length(buffer.get_int32());
  // record.set_table_name(buffer.get_string(record.get_table_name_length()));
}