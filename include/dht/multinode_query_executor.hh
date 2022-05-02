#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "partitioner.hh"
#include "result_serializer.hh"
#include "statement_executor.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC MultinodeQueryExecutor {

public:
  MultinodeQueryExecutor(const string &query,
                         const std::shared_ptr<Statement> &stmt)
      : m_query(query), m_stmt(stmt) {}

  string execute() {
    Partitioner partitioner;
    std::optional<Record> local_result = StatementExecutor(m_stmt).execute();
    // get result from all other nodes
    // ***

    partitioner.send_all(m_query);

    return "All nodes have been updated";
    return ResultSerializer(local_result).serialize();
  }

private:
  string m_query;
  std::shared_ptr<Statement> m_stmt;
};

} // namespace dibibase::dht
