#pragma once

#ifndef COMMITLOG_PROTOTYPE_LOG_RECORD_HH
#define COMMITLOG_PROTOTYPE_LOG_RECORD_HH
#include <memory>
#include <cstring>
#include <sstream>
#include <string>

#include "common.hh"
#include "util/buffer.hh"
/**
 * Log Record Structure:
 * Insert: (depricated)
 * ---------------------------------------------------------------------------------
 * | Log Size | Log Type | LSN | Previous LSN | TableID  | Entry Size | Entry Data |
 * | 4 bytes  | 4 bytes  | 4   |   4 bytes    | 4 bytes  |   4 bytes  | variable   |
 * |              20 bytes- HEADER                       |          body           |
 * ---------------------------------------------------------------------------------
 * Update:
 * ----------------------------------------------------------------------------------------------------
 * | Log Size | Log Type | LSN | Previous LSN | TableNameSize | TableName | Record Size | Record Data |
 * |  4 bytes | 4 bytes  |  4  |   4 bytes    |    4 bytes    | variable  |    4 bytes  |   variable  |
 * |              20 bytes- HEADER                            |                body                   |
 * ----------------------------------------------------------------------------------------------------
 * Delete:
 * ----------------------------------------------------------------
 * | Log Size | Log Type | LSN | Previous LSN | TableID | TupleID |
 * | 4 bytes  | 4 bytes  |  4  |   4 bytes    | 4 bytes | 4 bytes |
 * |              20 bytes- HEADER                      |   body  |
 * ----------------------------------------------------------------
 */
enum class LogRecordType {
  INVALID = 0,
  INSERT,
  UPDATE,
  DELETE
};
namespace commitlog{
class DIBIBASE_PUBLIC LogRecord {
public:
  // INSERT-type record constructor
  LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
            int32_t data_size, std::string data);

  // UPDATE-type record constructor
  LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
            int32_t tupleID, int32_t data_size, std::string data);

  // DELETE-type record constructor
  LogRecord(int32_t prev_lsn, LogRecordType log_record_type, const int32_t &tableID,
            int32_t tupleID);

  // Constructor from a buffer
  explicit LogRecord(std::unique_ptr<char[]> buffer);
  explicit LogRecord(dibibase::util::Buffer*);

  // barebones constructor
  explicit LogRecord(){};

  // constructor from util::buffer
  static LogRecord from_buffer(dibibase::util::Buffer &buffer);

  // construct header for record
  std::unique_ptr<char[]> construct_record_buffer();

  // getters & setters
  inline void set_lsn(int16_t number) { lsn = number; };

  inline int32_t get_size() const { return size; };

  inline LogRecordType get_type() const { return record_type; };

  inline int32_t get_lsn() const { return lsn; };

  inline int32_t get_previous_lsn() const { return previous_lsn; };

  inline int32_t get_table_name_length() const { return table_name_length; };

  inline int32_t get_tuple() const { return tuple_id; };

  inline int32_t get_data_size() const { return entry_size; };

  inline std::string get_data() const { return entry_data; };
// for testing
// std::string to_string();

  const static auto HEADER_SIZE = 20; // 4 + 4 + 4 + 4 + 4 : The first 5 fields are the same in all record types


  // record header offsets
  const static auto LOG_SIZE_OFFSET = 0;
  const static auto LOG_TYPE_OFFSET = 4;
  const static auto LSN_OFFSET = 8;
  const static auto PREVIOUS_LSN_OFFSET = 12;
  const static auto TABLE_NAME_SIZE_OFFSET = 16;
  const static auto TABLE_NAME_OFFSET = 20;

private:
  int32_t size = 0; // length of the entire record
  LogRecordType record_type = LogRecordType::INVALID;
  int32_t lsn = 0;
  int32_t previous_lsn = 0;
  int32_t tuple_id;
  int32_t entry_size;
  std::string entry_data;
  int32_t table_name_length;
  std::string table_name;

};

#endif //COMMITLOG_PROTOTYPE_LOG_RECORD_HH
} // namespace commitlog