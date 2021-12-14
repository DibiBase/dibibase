#include "db/sstable_wrapper.hh"

#include "util/logger.hh"
#include <bitset>
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unistd.h>

using namespace dibibase::db;

SSTableWrapper::SSTableWrapper(std::multimap<std::string, std::string> memtable)
    : m_memtable(memtable), m_sstable_id(0), m_logger(Logger::make()) {

  fetch_recent_sstable_id();

  //Creating new sstable ID.
  m_sstable_id++;

  char *buffer = new char[4096];
  size_t allocated_bytes = encode_data(buffer);
  m_files = std::unique_ptr<SSTableFiles>(
      new SSTableFiles(buffer, allocated_bytes, m_sstable_id));

  delete[] buffer;
}

SSTableWrapper::SSTableWrapper() : m_sstable_id(0), m_logger(Logger::make()) {
  fetch_recent_sstable_id();
}

void SSTableWrapper::fetch_recent_sstable_id() {
  m_fd_metadata = open("sstables/metadata.db", O_RDONLY);

  if (m_fd_metadata > 0) {
    char *mem_buff = new char[2];

    ssize_t read_bytes = read(m_fd_metadata, mem_buff, 2);
    if (read_bytes < 0) {
      m_logger.err("Failed to read metadata.");
    } else {
      m_sstable_id = mem_buff[0] + (mem_buff[1] << 8);
    }

    delete[] mem_buff;
  }
}

std::string SSTableWrapper::read_key_value_pair(std::string key) {
  // In case SSTables haven't been created yet.
  if (m_sstable_id == 0) {
    m_logger.info("No SSTables found.");
    return "";
  }

  std::string required_value;

  // Iterating through all SStables starting from the last created one.
  for (int i = m_sstable_id; i > 0; --i) {
    m_files = std::unique_ptr<SSTableFiles>(new SSTableFiles(i));
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
  ssize_t read_bytes = m_files->read_data_file(buffer);
  if (read_bytes <= 0) {
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
    uint16_t value_size =
        buffer[buffer_index] + (buffer[buffer_index + 1] << 8);
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

size_t SSTableWrapper::encode_data(char *buffer) {
  // Data Format:
  // First 2 bytes represents nu. of entires in memtable.
  // For each entry:
  // - 2 bytes for the key length.
  // - key characters, each takes 1 byte.
  // - 2 bytes for the value length.
  // - value characters, each takes 1 byte.

  size_t allocated_bytes = 0;

  uint16_t mem_size = m_memtable.size();
  char mem_buff[2];
  memset(mem_buff, 0, sizeof(mem_buff));
  memcpy(mem_buff, reinterpret_cast<char *>(&mem_size), 2);
  memcpy(buffer, mem_buff, 2);

  buffer += 2;
  allocated_bytes += 2;

  for (auto it : m_memtable) {
    // Storing key size in 2 bytes.
    uint16_t key_size = it.first.size();
    char key_buff[2];
    memset(key_buff, 0, sizeof(key_buff));
    memcpy(key_buff, reinterpret_cast<char *>(&key_size), 2);
    memcpy(buffer, key_buff, 2);

    buffer += 2;
    allocated_bytes += 2;

    // Storing key characters, each character in 1 byte.
    std::strcpy(buffer, it.first.c_str());
    buffer += key_size;
    allocated_bytes += key_size;

    // Storing value size in 2 bytes.
    uint16_t value_size = it.second.size();
    char value_buff[2];
    memset(value_buff, 0, sizeof(value_buff));
    memcpy(value_buff, reinterpret_cast<char *>(&value_size), 2);
    memcpy(buffer, value_buff, 2);

    buffer += 2;
    allocated_bytes += 2;

    // Storing value characters, each character in 1 byte.
    std::strcpy(buffer, it.second.c_str());
    buffer += value_size;
    allocated_bytes += value_size;
  }
  return allocated_bytes;
}

SSTableWrapper::~SSTableWrapper() {
  if (m_fd_metadata > 0) {
    if (close(m_fd_metadata) < 0) {
      m_logger.err("Failed to close metadata file: %d", m_fd_metadata);
    } else {
      m_logger.info("Metadata file is closed successfully");
    }
  }
}