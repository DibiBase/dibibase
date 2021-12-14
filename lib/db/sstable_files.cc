#include "db/sstable_files.hh"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <inttypes.h>

using namespace dibibase::db;

SSTableFiles::SSTableFiles(char *data, size_t size, uint32_t sstable_num)
    : m_data(data), m_size(size), m_sstable_num(sstable_num),
      m_dbname("dibibase"), m_file_descriptor{0, 0, 0, 0},
      m_logger(Logger::make()) {

  for (int index = 0; index < 4; ++index) {
    std::string file_name = get_file_name(index);

    m_file_descriptor[index] = open(file_name.c_str(), O_RDWR | O_CREAT,
                                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (m_file_descriptor[index] < 0) {
      m_logger.info("Failed to open file: %d", m_file_descriptor[index]);
    }
  }
  
  // Storing the last SSTable ID created in metadata.
  int fd_metadata = open("sstables/metadata.db", O_RDWR | O_CREAT,
                                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  char mem_buff[2];
  memset(mem_buff, 0, sizeof(mem_buff));
  memcpy(mem_buff, reinterpret_cast<char *>(&m_sstable_num), 2);
  write(fd_metadata, mem_buff, sizeof(mem_buff));

  ssize_t written_bytes = write_data_file();
  m_logger.info("Written Bytes: %d", written_bytes);
}

SSTableFiles::SSTableFiles(uint32_t sstable_num)
    : m_sstable_num(sstable_num), m_file_descriptor{0, 0, 0, 0}, m_logger(Logger::make()) {

  // Getting SSTable data file name.
  std::string file_name = get_file_name(DATA);
  // Opening SSTable data file.
  m_file_descriptor[DATA] = open(file_name.c_str(), O_RDONLY);
}

std::string SSTableFiles::get_file_name(int file_num) {

  // TO DO: fix an error results from using m_dbname in the file name.
  switch (file_num) {
  case DATA:
    return "sstables/data" + std::to_string(m_sstable_num) + ".db";
  case INDEX:
    return "sstables/index" + std::to_string(m_sstable_num) + ".db";
  case SUMMARY:
    return "sstables/summary" + std::to_string(m_sstable_num) + ".db";
  case COMPRESSION:
    return "sstables/compression_information" + std::to_string(m_sstable_num) + ".db";
  }

  throw "Invalid File Type Error";
}

ssize_t SSTableFiles::write_data_file() {
  uint32_t fd = m_file_descriptor[DATA];
  return write(fd, m_data, m_size);
}

ssize_t SSTableFiles::read_data_file(char *buffer) {
  uint32_t fd = m_file_descriptor[DATA];

  lseek(fd, 0, SEEK_SET);

  ssize_t read_bytes = read(fd, buffer, 4096);

  if (read_bytes < 0) {
    m_logger.err("Failed to read data.");
  }

  m_logger.info("read bytes is %d", read_bytes);
  return read_bytes;
}

SSTableFiles::~SSTableFiles() {
  for (uint8_t index = 0; index < 4; ++index) {
    int fd = m_file_descriptor[index];

    // In case the current file isn't opened.
    if(fd <= 0) {
      continue;
    }

    if (close(fd) < 0) {
      m_logger.err("Failed to close file: %d", fd);
    } else {
      m_logger.info("File is closed successfully");
    }
  }
}