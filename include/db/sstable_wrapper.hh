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
  explicit SSTableWrapper();

  // Loading SSTable data file into a buffer to read from.
  std::string decode_data(std::string key);

  // Reading key value pair from the existing SSTable files.
  std::string read_key_value_pair(std::string key);
  ~SSTableWrapper();

private:
  std::unique_ptr<SSTableFiles> m_fetched_files;
  int m_last_sstable_id;
  int m_fd_metadata;
  Logger m_logger;
};
} // namespace dibibase::db