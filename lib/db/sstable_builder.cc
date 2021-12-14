#include "db/sstable_builder.hh"

#include "util/logger.hh"
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <inttypes.h>

using namespace dibibase::db;

SSTableBuilder::SSTableBuilder(std::multimap<std::string, std::string> memtable)
    : m_memtable(memtable), m_sstable_id(0), m_logger(Logger::make()) {

  // Fetching the last SSTable ID.
  m_fd_metadata = open("sstables/metadata.db", O_RDONLY);

  if (m_fd_metadata > 0) {
    char *mem_buff = new char[2];

    ssize_t read_bytes = read(m_fd_metadata, mem_buff, 4096);
    if (read_bytes < 0) {
      m_logger.err("Failed to read metadata.");
    } else {
      m_sstable_id = mem_buff[0] + (mem_buff[1] << 8);
      ++m_sstable_id;
    }

    delete[] mem_buff;
  }

  char *buffer = new char[4096];
  size_t allocated_bytes = encode_data(buffer);
  m_files = std::unique_ptr<SSTableFiles>(
      new SSTableFiles(buffer, allocated_bytes, m_sstable_id));

  delete[] buffer;
}

size_t SSTableBuilder::encode_data(char *buffer) {
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

SSTableBuilder::~SSTableBuilder() {}
