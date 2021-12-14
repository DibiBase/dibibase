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
class DIBIBASE_PUBLIC SSTableWrapper {
public:
  explicit SSTableWrapper(std::multimap<std::string, std::string> memtable); 

  explicit SSTableWrapper();

  // Storing data from buffer into SSTable data file.
  size_t encode_data(char *buffer);

  // Loading SSTable data file into a buffer to read from.
  std::string decode_data(std::string key);

  // Reading key value pair from the existing SSTable files.
  std::string read_key_value_pair(std::string key);
  ~SSTableWrapper();

private:
  std::multimap<std::string, std::string> m_memtable;
  std::unique_ptr<SSTableFiles> m_files;
  int m_sstable_id;
  int m_fd_metadata;
  Logger m_logger;
  void fetch_recent_sstable_id();
};
} // namespace dibibase::db