#pragma once

#include "common.hh"

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"

#include "lang/cql3_visitor.hh"
#include "lang/statements/statement.hh"

using namespace antlr4;
using namespace dibibase::lang;
using namespace dibibase::lang::cql3;

namespace dibibase::dht {

class DIBIBASE_PUBLIC StatementParser {

public:
  StatementParser(const string &query) : m_query(query) {}

  std::unique_ptr<Statement> process() {
    // TODO: handle exceptions
    ANTLRInputStream input(m_query);
    CqlLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    CqlParser parser(&tokens);

    CqlParser::RootContext *tree = parser.root();
    CQL3Visitor visitor;
    std::vector<Statement *> statements =
        visitor.visitRoot(tree).as<std::vector<Statement *>>();

    return std::unique_ptr<Statement>(statements[0]);
  }

private:
  std::string m_query;
};

} // namespace dibibase::dht
