#pragma once

#include "common.hh"

#include "lang/statements/statement.hh"
#include "local_query_executor.hh"
#include "multinode_query_executor.hh"
#include "partitioner.hh"
#include "result_serializer.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC SinglenodeQueryExecutor {

public:
  SinglenodeQueryExecutor(const string &query,
                          const std::shared_ptr<Statement> &stmt)
      : m_query(query), m_stmt(stmt) {}

  string execute() {
    Partitioner partitioner;
    // TODO: handle partition key
    string key = m_stmt->partition_key().value();
    if (partitioner.is_local(key)) {
      return LocalQueryExecutor(m_stmt).execute();
    } else if (key.empty()) {
      return MultinodeQueryExecutor(m_query, m_stmt).execute();
    }
    return partitioner.send(m_query, key);
  }

private:
  string m_query;
  std::shared_ptr<Statement> m_stmt;
};

} // namespace dibibase::dht
