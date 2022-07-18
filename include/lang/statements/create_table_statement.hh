#pragma once

#include <optional>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "lang/statements/batch.hh"
#include "lang/statements/statement.hh"
#include "lang/statements/ttl_timestamp.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC CreateTableStatement : public Statement {

public:
  CreateTableStatement() : Statement(Type::CREATE_TABLE), m_schema(0, 0) {}

  CreateTableStatement(bool if_not_exists, std::string keyspace,
                       std::string table, catalog::Schema schema)
      : Statement(Type::CREATE_TABLE), m_if_not_exists(if_not_exists),
        m_keyspace(keyspace), m_table(table), m_schema(schema) {}

  std::optional<catalog::Record> execute(db::Database &db) override {
    db.create_table(m_table, m_schema);
    return std::nullopt;
  }

public:
  bool m_if_not_exists;
  std::string m_keyspace;
  std::string m_table;
  catalog::Schema m_schema;
};

} // namespace dibibase::lang