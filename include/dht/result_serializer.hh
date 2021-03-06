#pragma once

#include "common.hh"

#include "catalog/record.hh"

using namespace dibibase::catalog;

namespace dibibase::dht {

class DIBIBASE_PUBLIC ResultSerializer {

public:
  ResultSerializer(const std::optional<Record> &record) : m_record(record) {}

  string serialize() {
    if (m_record.has_value()) {
      string result;
      for (auto value : m_record.value().values())
        result += value->print() + "," + std::to_string(value->type().id()) + "," + std::to_string(value->type().length()) + "|";
      return result; // TODO: serialize to Rows
    }
    return ""; // TODO: Void
  }

private:
  const std::optional<Record> m_record;
};

} // namespace dibibase::dht
