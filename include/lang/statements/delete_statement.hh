#pragma once

#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "common.hh"
#include "lang/statements/batch.hh"
#include "lang/statements/statement.hh"
#include "lang/statements/ttl_timestamp.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC DeleteStatement : public Statement {

public:
  DeleteStatement() : Statement(Type::INSERT) {}

  DeleteStatement(std::string table, std::vector<catalog::Data *> values,
                  Batch batch = {}, std::string keyspace = "",
                  std::vector<std::string> columns = {},
                  bool if_not_exists = false, TtlTimestamp ttl_timestamp = {})
      : Statement(Type::INSERT), m_batch(batch), m_keyspace(keyspace),
        m_table(table), m_columns(columns), m_values(values),
        m_if_not_exists(if_not_exists), m_ttl_timestamp(ttl_timestamp) {}


  std::optional<catalog::Record> execute(db::Database &db) override {
    std::vector<std::shared_ptr<catalog::Data>> values;

    for (auto value : m_values) {
      values.push_back(catalog::Data::from(value));
    }

  // Add special value to mark as deleted
    db.write_record(m_table, catalog::Record(values));
    return std::nullopt;
  }

public:
  Batch m_batch;
  std::string m_keyspace;
  std::string m_table;
  std::vector<std::string> m_columns;
  std::vector<catalog::Data *> m_values;
  bool m_if_not_exists;
  TtlTimestamp m_ttl_timestamp;
};

} // namespace dibibase::lang