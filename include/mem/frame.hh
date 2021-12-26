#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <unistd.h>

#include "common.hh"
#include "errors.hh"
#include "io/manager.hh"

namespace dibibase::mem {

DEFINE_ERROR(invalid_operation);

class DIBIBASE_PUBLIC Frame {

public:
  Frame(std::unique_ptr<char[]> contents, uint64_t next_free) {
    Frame(std::move(contents), ~next_free, io::Manager::INVALID_PAGE_NUMBER);
  }

  Frame(Frame &frame) {
    Frame(std::move(frame.m_contents), frame.m_index, frame.m_page_number);
  }

  Frame(std::unique_ptr<char[]> contents, uint32_t index, uint64_t page_num)
      : m_contents(std::move(contents)), m_index(index),
        m_page_number(page_num), m_dirty(false) {}

  int64_t page_num() { return m_page_number; }

  bool valid() { return m_index > 0; }

  void pin() {
    // TODO: only pin if valid
  }
  void unpin() {}
  bool is_pinned();

  void flush() {
    pin();

    // TODO: only flush if valid and dirty
    // TODO: tell disk space manager to write to disk

    m_dirty = false;

    unpin();
  }

  void read(std::unique_ptr<char[]> buf, size_t nbytes, off_t offset) {
    pin();

    // TODO: copy from m_contents
    // TODO: hit

    unpin();
  }

  void write(std::unique_ptr<char[]> buf, size_t nbytes, off_t offset) {
    pin();

    // TODO: copy to m_contents
    // TODO: set dirty
    // TODO: hit

    unpin();
  }

  Frame request_valid_frame() {
    // TODO: if valid pin and return
    // TODO: else get a new from the manager
  }

private:
  static constexpr uint32_t INVALID_INDEX =
      std::numeric_limits<uint32_t>().min();

private:
  bool free() { return m_index < 0 && m_index != INVALID_INDEX; }

  void invalidate() {
    if (valid())
      flush();

    m_index = INVALID_INDEX;
    m_contents.reset();
  }

private:
  std::unique_ptr<char[]> m_contents;
  int m_index;
  long m_page_number;
  bool m_dirty;
};

} // namespace dibibase::mem
