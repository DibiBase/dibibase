#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.hh"
#include "query/predicate-operator.hh"

namespace dibibase::query {

class DIBIBASE_PUBLIC QueryPlan {

public:
  QueryPlan(std::string table_name, std::string table_alias);
  ~QueryPlan();

private:
  std::string m_table_name;
  PredicateOperator m_predicate_operator;
  std::vector<std::string> m_project_columns;
  std::unordered_map<std::string, std::string> m_aliases_map;
  std::string m_sort_column;
  size_t m_limit;
  size_t m_offset;
};

} // namespace dibibase::query
