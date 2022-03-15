#pragma once

#include <cstdint>

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC TimestampSpec {

public:
  TimestampSpec() : m_valid(false) {}

  TimestampSpec(uint64_t timestamp) : m_valid(true), m_timestamp(timestamp) {}

public:
  bool m_valid;
  uint64_t m_timestamp;
};

} // namespace dibibase::lang
