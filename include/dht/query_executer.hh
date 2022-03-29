#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "statement_executer.hh"
#include "statement_processor.hh"
#include "result_serializer.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC QueryExecuter {

public:
  QueryExecuter(const string &query) : m_query(query) {}

  string execute() {
    std::shared_ptr<Statement> stmt = StatementProcessor(m_query).process();
    std::optional<Record> result = StatementExecuter(stmt).execute();
    return ResutlSerializer(result).serialize();
  }

private:
  const string m_query;
};

} // namespace dibibase::dht
