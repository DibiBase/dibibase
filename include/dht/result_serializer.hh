#pragma once

#include "common.hh"

#include "catalog/record.hh"

using namespace dibibase::catalog;

namespace dibibase::dht {

class DIBIBASE_PUBLIC ResutlSerializer {

public:
  ResutlSerializer(const std::optional<Record> &record) : m_record(record) {}

  string serialize() {
    if (m_record.has_value()) {
      return ""; // TODO: serialize to Rows
    }
    return ""; // TODO: Void
  }

private:
  const std::optional<Record> m_record;
};

} // namespace dibibase::dht
