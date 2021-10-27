#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "common.hh"

namespace dibibase::db {

enum FileType {
  dataFile,
  indexFile,
  indexSummaryFile,
  compressionInformationFile
};

class DIBIBASE_PUBLIC SSTableFiles {
public:
  explicit SSTableFiles(char *data, uint8_t sstable_num);
  std::string get_file_name(uint8_t file_num);
  std::size_t write_data_file();
  void read_data_file(char *buffer);
  ~SSTableFiles();

private:
  char *m_data;
  size_t m_written_bytes;
  uint8_t m_sstable_num;
  const std::string m_dbname;
  uint32_t m_file_descriptor[4];
};
} // namespace dibibase::db