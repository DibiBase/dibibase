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
  size_t encode_data(char *buffer);
  ~SSTableBuilder();

private:
  std::multimap<std::string, std::string> m_memtable;
  std::unique_ptr<SSTableFiles> m_files;
  int m_sstable_id;
  int m_fd_metadata;
  Logger m_logger;
};
} // namespace dibibase::db
