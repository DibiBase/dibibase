#include "db/sstable_files.hh"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>

using namespace dibibase::db;

SSTableFiles::SSTableFiles(char *data, uint8_t sstable_num)
    : m_data(data), m_sstable_num(sstable_num), m_dbname("dibibase") {

  for (uint8_t index = 0; index < 4; ++index) {
    std::string file_name = get_file_name(index);

    m_file_descriptor[index] =
        open(file_name.c_str(), O_RDWR | O_CREAT,
             S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    if (m_file_descriptor[index] < 0) {
      std::cout << "Failed to open file: " << m_file_descriptor[index]
                << std::endl;
    }
  }

  m_written_bytes = write_data_file();
}

std::string SSTableFiles::get_file_name(std::uint8_t file_num) {

  // TO DO: fix an error results from using m_dbname in the file name.
  switch (file_num) {
  case dataFile:
    return "data" + std::to_string(m_sstable_num) + ".db";
  case indexFile:
    return "index" + std::to_string(m_sstable_num) + ".db";
  case indexSummaryFile:
    return "summary" + std::to_string(m_sstable_num) + ".db";
  case compressionInformationFile:
    return "compression_information" + std::to_string(m_sstable_num) + ".db";
  }

  return "";
}

size_t SSTableFiles::write_data_file() {
  uint32_t fd = m_file_descriptor[dataFile];
  return write(fd, m_data, strlen(m_data));
}

void SSTableFiles::read_data_file(char *buffer) {
  uint32_t fd = m_file_descriptor[dataFile];

  lseek(fd, 0, SEEK_SET);

  if (read(fd, buffer, m_written_bytes) < 0) {
    std::cout << "Failed to read data." << std::endl;
  }
}

SSTableFiles::~SSTableFiles() {
  for (uint8_t index = 0; index < 4; ++index) {
    if (close(m_file_descriptor[index]) < 0) {
      std::cout << "Failed to close file: " << m_file_descriptor[index]
                << std::endl;
    } else {
      std::cout << "File is closed successfully" << std::endl;
    }
  }
}