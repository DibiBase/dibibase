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

SSTableFiles::SSTableFiles(char *data, size_t size, uint32_t sstable_num)
    : m_data(data), m_size(size), m_sstable_num(sstable_num),
      m_dbname("dibibase"), m_logger(Logger::make()) {

  for (int index = 0; index < 4; ++index) {
    std::string file_name = get_file_name(index);

    m_file_descriptor[index] = open(file_name.c_str(), O_RDWR | O_CREAT,
                                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (m_file_descriptor[index] < 0) {
      m_logger.info("Failed to open file: %d", m_file_descriptor[index]);
    }
  }

  ssize_t written_bytes = write_data_file();
  m_logger.info("Written Bytes: %d", written_bytes);
}

std::string SSTableFiles::get_file_name(int file_num) {

  // TO DO: fix an error results from using m_dbname in the file name.
  switch (file_num) {
  case DATA:
    return "data" + std::to_string(m_sstable_num) + ".db";
  case INDEX:
    return "index" + std::to_string(m_sstable_num) + ".db";
  case SUMMARY:
    return "summary" + std::to_string(m_sstable_num) + ".db";
  case COMPRESSION:
    return "compression_information" + std::to_string(m_sstable_num) + ".db";
  }

  throw "Invalid File Type Error";
}

ssize_t SSTableFiles::write_data_file() {
  uint32_t fd = m_file_descriptor[DATA];
  return write(fd, m_data, m_size);
}

void SSTableFiles::read_data_file(char *buffer) {
  uint32_t fd = m_file_descriptor[DATA];

  lseek(fd, 0, SEEK_SET);

  if (read(fd, buffer, m_size) < 0) {
    m_logger.err("Failed to read data.");
  }
}

SSTableFiles::~SSTableFiles() {
  for (uint8_t index = 0; index < 4; ++index) {
    if (close(m_file_descriptor[index]) < 0) {
      m_logger.err("Failed to close file: %d", m_file_descriptor[index]);
    } else {
      m_logger.info("File is closed successfully");
    }
  }
}
