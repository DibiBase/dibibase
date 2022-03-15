#pragma once

#include <string>
#include <vector>

#include "common.hh"
#include "lang/statements/batch.hh"
#include "lang/statements/from_spec.hh"
#include "lang/statements/order_spec.hh"
#include "lang/statements/select_elements.hh"
#include "lang/statements/statement.hh"
#include "lang/statements/ttl_timestamp.hh"
#include "lang/statements/where_sepc.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC SelectStatement : public Statement {

public:
  SelectStatement() : Statement(Type::SELECT) {}

  SelectStatement(SelectElements select_elements, FromSpec from_spec,
                  bool distinct = false, bool json = false,
                  WhereSpec where_spec = {}, OrderSpec order_spec = {},
                  uint64_t limit = 0, bool allow_filtering = false)
      : Statement(Type::SELECT), m_distinct(distinct), m_json(json),
        m_select_elements(select_elements), m_from_spec(from_spec),
        m_where_spec(where_spec), m_order_sepc(order_spec), m_limit(limit),
        m_allow_filtering(allow_filtering) {}

public:
  bool m_distinct;
  bool m_json;
  SelectElements m_select_elements;
  FromSpec m_from_spec;
  WhereSpec m_where_spec;
  OrderSpec m_order_sepc;
  uint64_t m_limit;
  bool m_allow_filtering;
};

} // namespace dibibase::lang