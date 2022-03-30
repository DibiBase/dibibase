#pragma once

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"

#include "streamer.grpc.pb.h"

#include "lang/statements/statement.hh"
#include "partitioner.hh"
#include "statement_parser.hh"

using dibibase::catalog::Record;
namespace dibibase::dht {

class DIBIBASE_PUBLIC QueryProcessor {

public:
  QueryProcessor(const string &query) : m_query(query) {}

  string process() {
    Partitioner partitioner;

    std::shared_ptr<Statement> stmt = StatementParser(m_query).process();

    switch (stmt->type()) {
    case Statement::Type::CREATE_TABLE:
    case Statement::Type::DROP_TABLE:
    case Statement::Type::CREATE_INDEX:
    case Statement::Type::DROP_INDEX: {
      partitioner.send_all(m_query);

      return "All nodes have been updated";
      // TODO: process local statement in local flow
    }
    case Statement::Type::INSERT:
    case Statement::Type::SELECT: {
      // TODO: handle partition key
      string key = stmt->partition_key().value();
      if (partitioner.is_local(key)) {
        return process_local(stmt);
      }
      return partitioner.send(m_query, key);
    }
    default: {
      break;
    }
    }
  }

private:
  string process_local(const std::shared_ptr<Statement> &stmt) {
    return ResultSerializer(StatementExecutor(stmt).execute()).serialize();
  }

private:
  const string m_query;
};

} // namespace dibibase::dht
