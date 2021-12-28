#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "common.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC Range {

public:
  Range(uint64_t start, uint64_t end, std::string name);

  uint64_t start() const { return m_start; }
  uint64_t end() const { return m_end; }
  std::string address() const { return m_address; }

  bool contains(uint64_t id) const { return m_start <= id && id < m_end; }

  bool operator==(const Range &other) const;
  bool operator!=(const Range &other) const;

private:
  uint64_t m_start;
  uint64_t m_end;
  std::string m_address;
};

} // namespace dibibase::dht