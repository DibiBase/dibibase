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

class DIBIBASE_PUBLIC DrobTableStatement : public Statement {

public:
  DrobTableStatement() : Statement(Type:: DROP_TABLE) {}

  DrobTableStatement(bool if_not_exists, std::string keyspace,
                       std::string table)
      : Statement(Type:: DROP_TABLE), m_if_not_exists(if_not_exists),
        m_keyspace(keyspace), m_table(table){}

  std::optional<catalog::Record> execute(db::Database &db) override {
    db.delete_table(m_table);
    return std::nullopt;
  }

public:
  bool m_if_not_exists;
  std::string m_keyspace;
  std::string m_table;
};

} // namespace dibibase::lang
