#include "db/db.hh"

#include <cstdint>
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
  if (!m_memtable.empty()) {
    auto find_key = m_memtable.find(key);

    // TODO: In case having the same key multiple times, return the value of the
    // last occurance key.
    if (find_key != m_memtable.end()) {
      return find_key->second;
    }
  }

  for (int i = m_sstables.size() - 1; i >= 0; --i) {
    std::multimap<std::string, std::string> extracted_data =
        m_sstables[i]->decode_data();

    auto find_key = extracted_data.find(key);

    // TODO: In case having the same key multiple times, return the value of the
    // last occurance key.
    if (find_key != extracted_data.end()) {
      return find_key->second;
    }
  }

  return "";
}

void DBImpl::remove(std::string key) {}

void DBImpl::flush() {
  m_sstables.push_back(std::make_unique<SSTableBuilder>(m_memtable));
  clear_memtable();
}

size_t DBImpl::get_memtable_size() {
  return sizeof(std::string) + sizeof(std::string) * m_memtable.size();
}

void DBImpl::clear_memtable() { m_memtable.clear(); }
