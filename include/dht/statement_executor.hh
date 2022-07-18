#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "state_store.hh"

using namespace dibibase::lang;
using dibibase::catalog::Record;
namespace dibibase::dht {

class DIBIBASE_PUBLIC StatementExecutor {

public:
  StatementExecutor(const std::shared_ptr<Statement> &statement)
      : m_statement(statement) {}

  std::optional<Record> execute() {
    // TODO: handle exceptions
    string address = Config::instance().local_address();
    auto keyspace = StateStore::instance().get_keyspace("data/" + address + ":database");
    std::optional<Record> result = m_statement->execute(*keyspace);

    if (result.has_value())
      for (auto value : result.value().values())
        // std::cout << value->print() << std::endl;
    return result;
  }

private:
  std::shared_ptr<Statement> m_statement;
};

} // namespace dibibase::dht
