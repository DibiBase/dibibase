#pragma once

#include <string>
#include <vector>

#include "common.hh"

namespace dibibase::lang::cql3 {

class DIBIBASE_PUBLIC InsertStatement {
public:
  std::string m_table;
  std::vector<std::string> m_columns;
  std::vector<std::string> m_values;
};

} // namespace dibibase::lang::cql3
