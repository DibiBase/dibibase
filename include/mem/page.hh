#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <unistd.h>

#include "common.hh"
#include "mem/frame.hh"

namespace dibibase::mem {

class DIBIBASE_PUBLIC Page {

public:
  Page(Frame frame) : m_frame(frame) {}

  Page(Page &page) : m_frame(page.m_frame) {}

  int64_t page_number() {}

  void pin() {}

  void unpin() {}

  void read(std::unique_ptr<char[]> buf, size_t nbytes, off_t offset) {}

  void write(std::unique_ptr<char[]> buf, size_t nbytes, off_t offset) {}

  void wipe() {}

  void flush() {}

private:
  Frame m_frame;
};

} // namespace dibibase::mem
