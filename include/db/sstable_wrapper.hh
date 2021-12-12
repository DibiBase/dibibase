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

  // Fetching the recent/latest SSTable files number.
  static void set_last_sstable_id(int sstable_number);

  // Loading SSTable data file into a buffer to read from.
  std::string decode_data(std::string key);

  // Reading key value pair from the existing SSTable files.
  std::string read_key_value_pair(std::string key);
  ~SSTableWrapper();

private:
  static int m_last_sstable_id;
  std::unique_ptr<SSTableFiles> m_fetched_files;
  Logger m_logger;
};
} // namespace dibibase::db