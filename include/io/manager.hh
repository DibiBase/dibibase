#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unistd.h>
#include <unordered_map>

#include "common.hh"
#include "io/partition.hh"

namespace dibibase::io {

class DIBIBASE_PUBLIC Manager {
public:
  static constexpr uint16_t PAGE_SIZE = 4096; // size of a page in bytes
  static constexpr int64_t INVALID_PAGE_NUMBER = -1L;
  static constexpr uint16_t MAX_NUMBER_OF_HEADER_PAGES =
      PAGE_SIZE / 2; // 2 bytes per header page
  static constexpr uint16_t DATA_PAGES_PER_HEADER =
      PAGE_SIZE * 8; // 1 bit per data page

public:
  Manager(std::string path) {
    // TODO: open dir, popluate m_partition_map, and count number of files to
    // set m_counter
  }

  ~Manager() {
    // TODO: close dir
  }

  ssize_t allocate_partition() {
    // TODO: get and update m_counter
    // return allocate_partition(m_counter);
  }

  ssize_t allocate_partition(uint32_t partition_number) {
    // TODO: create a new partition and update m_partition_map
  }

  ssize_t delete_partition(ssize_t partition_number) {
    // TODO: clear partition and delete its file
  }

  int64_t allocate_page(uint32_t partition_number);

  int64_t allocate_page(int64_t page_number);

  void delete_page(int64_t page_number);

  void read_page(int64_t page_number, std::unique_ptr<char[]> buf);

  void write_page(int64_t page_number, std::unique_ptr<char[]> buf);

  bool page_allocated(int64_t page_number);

private:
  Partition partition(uint32_t pratition_number);

  static int64_t get_page_number(int64_t page) { return page % 10000000000L; }
  static int64_t get_virtual_page_num(int64_t partition_number,
                                      int64_t page_number) {
    return partition_number * 10000000000L + page_number;
  }

private:
  int m_path;
  std::unordered_map<uint32_t, Partition> m_partition_map;
  size_t m_counter;
};

} // namespace dibibase::io
