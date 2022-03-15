#pragma once

#include <cstdint>

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC TtlTimestamp {

public:
  TtlTimestamp() : m_ttl_valid(false), m_timestamp_valid(false) {}

  static TtlTimestamp from_ttl(uint64_t ttl) {
    TtlTimestamp ttl_timestamp;

    ttl_timestamp.m_ttl_valid = true;
    ttl_timestamp.m_ttl = ttl;

    return ttl_timestamp;
  }

  static TtlTimestamp from_timestamp(uint64_t timestamp) {
    TtlTimestamp ttl_timestamp;

    ttl_timestamp.m_timestamp_valid = true;
    ttl_timestamp.m_timestamp = timestamp;

    return ttl_timestamp;
  }

  static TtlTimestamp from_ttl_and_timestamp(uint64_t ttl, uint64_t timestamp) {
    TtlTimestamp ttl_timestamp;

    ttl_timestamp.m_ttl_valid = true;
    ttl_timestamp.m_ttl = ttl;
    ttl_timestamp.m_timestamp_valid = true;
    ttl_timestamp.m_timestamp = timestamp;

    return ttl_timestamp;
  }

public:
  bool m_ttl_valid;
  uint64_t m_ttl;
  bool m_timestamp_valid;
  uint64_t m_timestamp;
};

} // namespace dibibase::lang
