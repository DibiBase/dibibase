#pragma once

#include <cstddef>
#include <cstdint>
#include <unistd.h>

#include "common.hh"
#include "errors.hh"
#include "io/manager.hh"

namespace dibibase::io {

DEFINE_ERROR(page_fault);

// A Partition has:
// 1. A master_page containing the count of used data_pages in each header_page.
// 2. heaer_pages containing a bitmap of used data_pages.
// 3. data_pages
// page offset:  0  1  2  3  4  5  6  7  8  9
// page type:   [M][H][D][D][D][D][H][D][D][D]
class DIBIBASE_PUBLIC Partition {

public:
  Partition(uint32_t number) : m_number(number) {
    // TODO: open file
    // TODO: create a master_page if not-existent
    // TODO: read master_page and header_pages
  }

  ~Partition() {
    // TODO: close file
  }

  ssize_t allocate_page() {
    // TODO: search master_page for a header_page that contains an empty
    // data_page
    // TODO: search header_page for an empty data_page
    // return allocate_page(header_idx, page_idx);
  }

  ssize_t allocate_page(uint32_t header_idx, int64_t page_idx) {
    // TODO: create new header_page if not-existent
    // TODO: set data_page bit and increase header_page count
    // TODO: flush mater_page
    // TODO: flush header_page

    return page_idx + header_idx * Manager::DATA_PAGES_PER_HEADER;
  }

  void delete_page(int64_t page_number) {
    uint32_t header_idx = page_number / Manager::DATA_PAGES_PER_HEADER;
    size_t page_idx = page_number % Manager::DATA_PAGES_PER_HEADER;

    // TODO: set data_page bit and increase header_page count
    // TODO: flush mater_page
    // TODO: flush header_page
  }

  void write_page(int64_t page_number, std::unique_ptr<char[]> buf);

  bool not_allocated(int64_t page_number);

  void clear();

private:
  void write_master_page();
  void write_header_page(uint32_t header_idx);

  static off_t get_master_page_offset() { return 0; }
  static off_t get_header_page_offset(uint32_t header_idx);
  static off_t get_data_page_offset(int64_t page_number);

private:
  int m_file;
  uint64_t m_number;
};

} // namespace dibibase::io
