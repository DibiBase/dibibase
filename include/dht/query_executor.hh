#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "statement_executor.hh"
#include "statement_parser.hh"
#include "result_serializer.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC QueryExecutor {

public:
  QueryExecutor(const string &query) : m_query(query) {}

  string execute() {
    std::shared_ptr<Statement> stmt = StatementParser(m_query).process();
    std::optional<Record> result = StatementExecutor(stmt).execute();
    return ResultSerializer(result).serialize();
  }

private:
  const string m_query;
};

} // namespace dibibase::dht
