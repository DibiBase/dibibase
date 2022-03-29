#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include <cstddef>
#include <iostream>
#include <optional>

using namespace dibibase::lang;
using dibibase::catalog::Record;

namespace dibibase::dht {

class DIBIBASE_PUBLIC StatementExecuter {

public:
  StatementExecuter(const std::shared_ptr<Statement> &statement)
      : m_statement(statement) {}

  std::optional<Record> execute() {
    db::Database db("database");

    // TODO: handle exceptions
    std::optional<Record> result = m_statement->execute(db);

    if (result.has_value())
      for (auto value : result.value().values())
        std::cout << value->print() << std::endl;
    return result;
  }

private:
  std::shared_ptr<Statement> m_statement;
};

} // namespace dibibase::dht
