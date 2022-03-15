#pragma once

#include <string>

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC FromSpec {

public:
  FromSpec(std::string table = "", std::string keyspace = "")
      : m_keyspace(keyspace), m_table(table) {}

public:
  std::string m_keyspace;
  std::string m_table;
};

} // namespace dibibase::lang
