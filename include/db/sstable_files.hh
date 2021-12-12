#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "common.hh"
#include "util/logger.hh"

using namespace dibibase::util;

namespace dibibase::db {

enum FileType {
  DATA, 
  INDEX,
  SUMMARY, 
  COMPRESSION
};

class DIBIBASE_PUBLIC SSTableFiles {
public:
  explicit SSTableFiles(char *data, size_t size, uint32_t sstable_num);
  explicit SSTableFiles(uint32_t sstable_num);
  std::string get_file_name(int file_num);
  ssize_t write_data_file();
  ssize_t read_data_file(char *buffer);
  ~SSTableFiles();

private:
  char *m_data;
  size_t m_size;
  uint32_t m_sstable_num;
  const std::string m_dbname;
  int m_file_descriptor[4];
  Logger m_logger;
};
} // namespace dibibase::db
