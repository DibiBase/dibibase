#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "result_serializer.hh"
#include "statement_executor.hh"
#include "statement_parser.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC LocalQueryExecutor {

public:
  LocalQueryExecutor(const std::shared_ptr<Statement> &stmt) : m_stmt(stmt) {}

  string execute() {
    std::optional<Record> result = StatementExecutor(m_stmt).execute();
    return ResultSerializer(result).serialize();
  }

private:
  std::shared_ptr<Statement> m_stmt;
};

} // namespace dibibase::dht
