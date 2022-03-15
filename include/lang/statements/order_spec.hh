#pragma once

#include <string>

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC OrderSpec {

public:
  OrderSpec() : m_valid(false) {}

  OrderSpec(std::string column, bool ascending = false)
      : m_valid(true), m_column(column), m_ascending(ascending) {}

public:
  bool m_valid;
  std::string m_column;
  bool m_ascending;
};

} // namespace dibibase::lang
