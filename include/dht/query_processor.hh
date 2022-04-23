#pragma once

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"

#include "streamer.grpc.pb.h"

#include "lang/statements/statement.hh"
#include "multinode_query_executor.hh"
#include "partitioner.hh"
#include "singlenode_query_executor.hh"
#include "statement_parser.hh"

using dibibase::catalog::Record;
namespace dibibase::dht {

class DIBIBASE_PUBLIC QueryProcessor {

public:
  QueryProcessor(const string &query) : m_query(query) {}

  string process() {

    std::shared_ptr<Statement> stmt = StatementParser(m_query).process();

    switch (stmt->type()) {
    case Statement::Type::CREATE_TABLE:
    case Statement::Type::DROP_TABLE:
    case Statement::Type::CREATE_INDEX:
    case Statement::Type::DROP_INDEX:
      return MultinodeQueryExecutor(m_query, stmt).execute();
    case Statement::Type::INSERT:
    case Statement::Type::SELECT:
      return SinglenodeQueryExecutor(m_query, stmt).execute();
    default:
      break;
    }
  }

private:
  const string m_query;
};

} // namespace dibibase::dht
