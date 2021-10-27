#include "db/sstable_builder.hh"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <type_traits>
#include <unistd.h>

using namespace dibibase::db;

uint8_t SSTableBuilder::m_total_sstables = 0;

SSTableBuilder::SSTableBuilder(std::multimap<std::string, std::string> memtable)
    : m_memtable(memtable) {
  ++m_total_sstables;
  char *buffer = new char[100];
  encode_data(buffer);
  m_files =
      std::unique_ptr<SSTableFiles>(new SSTableFiles(buffer, m_total_sstables));
  delete[] buffer;
}

void SSTableBuilder::encode_data(char *buffer) {
  // data format:
  // first byte represents nu. of entires in memtable.
  // for each entry: [key length, key characters, value length, value
  // characters].

  std::string mem_size = std::to_string(m_memtable.size());
  std::strcpy(buffer, mem_size.c_str());

  buffer += strlen(mem_size.c_str());

  for (auto it : m_memtable) {
    std::string key_size = std::to_string(it.first.size());
    std::string value_size = std::to_string(it.second.size());
    std::string key = it.first;
    std::string value = it.second;

    std::strcpy(buffer, key_size.c_str());
    buffer += strlen(key_size.c_str());
    std::strcpy(buffer, key.c_str());
    buffer += strlen(key.c_str());

    std::strcpy(buffer, value_size.c_str());
    buffer += strlen(value_size.c_str());
    std::strcpy(buffer, value.c_str());
    buffer += strlen(value.c_str());
  }
}

std::multimap<std::string, std::string> SSTableBuilder::decode_data() {
  char *buffer = new char[100];
  m_files->read_data_file(buffer);

  std::multimap<std::string, std::string> extracted_data;

  uint32_t size = buffer[0] - '0';

  uint8_t buffer_index = 1;

  for (uint32_t i = 0; i < size; ++i) {
    uint32_t key_size = buffer[buffer_index] - '0';

    ++buffer_index;
    std::string key(buffer + buffer_index, key_size);

    buffer_index += key_size;

    uint32_t value_size = buffer[buffer_index] - '0';
    ++buffer_index;

    std::string value(buffer + buffer_index, value_size);

    buffer_index += value_size;

    extracted_data.insert({key, value});
  }

  delete[] buffer;

  return extracted_data;
}

uint8_t SSTableBuilder::get_total_sstables() { return m_total_sstables; }

SSTableBuilder::~SSTableBuilder() { --m_total_sstables; }