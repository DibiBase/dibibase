#include "db/sstable_wrapper.hh"

#include "util/logger.hh"
#include <bitset>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unistd.h>

using namespace dibibase::db;

int SSTableWrapper::m_last_sstable_id = 2;

SSTableWrapper::SSTableWrapper() : m_logger(Logger::make()) {}

std::string SSTableWrapper::read_key_value_pair(std::string key) {
  // In case SSTables haven't been created yet.
  if (m_last_sstable_id == 0) {
    m_logger.info("No SSTables found.");
    return "";
  }

  std::string required_value;

  // Fetching the key value pair from the recent SSTable.
  for (int i = m_last_sstable_id; i > 0; --i) {
    m_fetched_files = std::unique_ptr<SSTableFiles>(new SSTableFiles(i));
    std::string value = decode_data(key);

    // In case key is found in the current SSTable.
    if (!value.empty()) {
      required_value = value;
      break;
    }
  }

  return required_value;
}

std::string SSTableWrapper::decode_data(std::string required_key) {

  char *buffer = new char[4096];
  ssize_t read_bytes = m_fetched_files->read_data_file(buffer);
  if(read_bytes <= 0) {
    return "";
  }
  
  std::string required_value;
  uint16_t size = buffer[0] + (buffer[1] << 8);

  int buffer_index = 2;

  for (uint32_t i = 0; i < size; ++i) {
    // Extracting the key size.
    uint16_t key_size = buffer[buffer_index] + (buffer[buffer_index + 1] << 8);
    buffer_index += 2;

    // Extracting the key.
    std::string key(buffer + buffer_index, key_size);
    buffer_index += key_size;

    // Extracting the key size.
    uint16_t value_size = buffer[buffer_index] + (buffer[buffer_index + 1] << 8);
    buffer_index += 2;

    // Extracting the value.
    std::string value(buffer + buffer_index, value_size);
    buffer_index += value_size;

    // In case key is found in the buffer.
    if (key == required_key) {
      required_value = value;
      break;
    }
  }

  delete[] buffer;
  return required_value;
}

void SSTableWrapper::set_last_sstable_id(int sstable_number) {
  m_last_sstable_id = sstable_number;
}

SSTableWrapper::~SSTableWrapper() {}