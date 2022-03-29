#pragma once

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"

#include "streamer.grpc.pb.h"

#include "lang/statements/statement.hh"
#include "partitioner.hh"
#include "statement_processor.hh"
#include <iostream>

using dibibase::catalog::Record;
namespace dibibase::dht {

class DIBIBASE_PUBLIC QueryProcessor {

public:
  QueryProcessor(const string &query) : m_query(query) {}

  string process() {
    Partitioner partitioner;

    std::unique_ptr<Statement> stmt = StatementProcessor(m_query).process();

    switch (stmt->type()) {
    case Statement::Type::CREATE_TABLE:
    case Statement::Type::DROP_TABLE:
    case Statement::Type::CREATE_INDEX:
    case Statement::Type::DROP_INDEX: {
      // partitioner.send_all(m_query);
      QueryExecuter(m_query).execute();
      break;
    }
    case Statement::Type::INSERT:
    case Statement::Type::SELECT: {
      // TODO: handle partition key
      string key = stmt->partition_key().value();
      std::cout << "Inserting into local table: " << key << std::endl;
      QueryExecuter(m_query).execute();

      break;
    }
    default: {
      break;
    }
    }
  }

private:
  const string m_query;
};

} // namespace dibibase::dht
