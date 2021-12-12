#include "db/db.hh"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

using namespace dibibase::db;

void DBImpl::write(std::string key, std::string value) {
  m_memtable.insert({key, value});

  // TO DO: use get_memtable_size()
  if (m_memtable.size() >= MAX_MEMTABLE_SIZE) {
    flush();
  }
}

std::string DBImpl::read(std::string key) {
  // Searching for the key value in memtable first.
  if (!m_memtable.empty()) {
    auto find_key = m_memtable.find(key);

    // TODO: In case having the same key multiple times, return the value of the
    // last occurance key.
    if (find_key != m_memtable.end()) {
      return find_key->second;
    }
  }

  // Key isn't found in memtable.
  // Searching for the key value in the existing SSTables.
  SSTableWrapper sstable_wrapper;
  std::string value = sstable_wrapper.read_key_value_pair(key);

  return value;
}

void DBImpl::remove(std::string key) {}

void DBImpl::flush() {
  SSTableBuilder sstable_builder(m_memtable);
  clear_memtable();
}

size_t DBImpl::get_memtable_size() {
  return sizeof(std::string) + sizeof(std::string) * m_memtable.size();
}

void DBImpl::clear_memtable() { m_memtable.clear(); }
