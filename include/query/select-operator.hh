#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "catalog/data.hh"
#include "common.hh"
#include "query/predicate-operator.hh"

namespace dibibase::query {

class DIBIBASE_PUBLIC SelectOperator {

public:
  SelectOperator(size_t column_index, std::string column_name,
                 PredicateOperator predicate_operator,
                 std::unique_ptr<catalog::Data> value);
  ~SelectOperator();

private:
  size_t m_column_index;
  std::string m_column_name;
  PredicateOperator m_predicate_operator;
  std::unique_ptr<catalog::Data> m_value;
};

} // namespace dibibase::query
