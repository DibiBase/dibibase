#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "common.hh"
#include "db/sstable_files.hh"

namespace dibibase::db {

class DIBIBASE_PUBLIC SSTableBuilder {
public:
  explicit SSTableBuilder(std::multimap<std::string, std::string> memtable);
  static uint8_t get_total_sstables();
  size_t encode_data(char *buffer);
  std::multimap<std::string, std::string> decode_data();
  ~SSTableBuilder();

private:
  std::multimap<std::string, std::string> m_memtable;
  std::unique_ptr<SSTableFiles> m_files;
  static uint32_t m_total_sstables;
};
} // namespace dibibase::db
