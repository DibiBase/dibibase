#pragma once

#include <vector>

#include "common.hh"

namespace dibibase::catalog {
class DIBIBASE_PUBLIC Schema;
}

#include "catalog/data.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Record {

public:
  class DIBIBASE_PUBLIC Id {

  public:
    Id(uint64_t page_number, off_t offset);

    static Id from(util::Buffer);

    uint64_t get_page_num() { return m_page_number; }
    off_t get_offset() { return m_offset; }

    size_t size() const;
    std::unique_ptr<char[]> bytes() const;

  private:
    uint64_t m_page_number;
    uint16_t m_offset;
  };

public:
  Record(std::vector<Data> values) : m_values(values){};

  static Record from(util::Buffer, Schema);
  template <class... Values> static Record from(Values &&...) {}

  const std::vector<Data> values() const { return m_values; }
  const Data &operator[](std::vector<Data>::size_type idx) const {
    return m_values[idx];
  }

  Record &operator+=(const Record &rhs) {
    m_values.insert(m_values.begin(), rhs.values().begin(), rhs.values().end());
    return *this;
  }
  friend Record operator+(Record lhs, const Record &rhs) {
    lhs += rhs;
    return lhs;
  }

  size_t size() const;
  std::unique_ptr<char[]> bytes() const;

private:
  std::vector<Data> m_values;
};

} // namespace dibibase::catalog
