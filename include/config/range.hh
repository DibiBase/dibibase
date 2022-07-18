#pragma once

#include "common.hh"

namespace dibibase::config {

class DIBIBASE_PUBLIC Range {

public:
  Range(uint64_t start, uint64_t end, std::string address)
      : m_start(start), m_end(end), m_address(address) {}

  uint64_t start() const { return m_start; }
  uint64_t end() const { return m_end; }
  std::string address() const { return m_address; }

  bool contains(uint64_t id) const { return m_start <= id && id < m_end; }

  bool operator==(const Range &other) const {
    return m_start == other.m_start && m_end == other.m_end &&
           m_address == other.m_address;
  }
  bool operator!=(const Range &other) const { return !(*this == other); }

private:
  uint64_t m_start;
  uint64_t m_end;
  std::string m_address;
};

} // namespace dibibase::dht
