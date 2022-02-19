#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Record {
public:
  Record(std::vector<Data> values) : m_values(values){};

  static std::unique_ptr<Record> from(util::Buffer *, Schema);

  const std::vector<Data> values() const { return m_values; }

  const Data &operator[](std::vector<Data>::size_type idx) const {
    return m_values[idx];
  }

  std::string to_string() const;

  Record &operator+=(const Record &rhs) {
    m_values.insert(m_values.begin(), rhs.values().begin(), rhs.values().end());
    return *this;
  }
  friend Record operator+(Record lhs, const Record &rhs) {
    lhs += rhs;
    return lhs;
  }

  size_t size() const;
  
  void bytes(util::Buffer *) const;

private:
  std::vector<Data> m_values;
};

} // namespace dibibase::catalog