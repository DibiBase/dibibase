#include <cstdlib>
#include <iostream>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"

#include "lang/cql3/InsertStatementVisitor.hh"
#include "lang/cql3/statements/InsertStatement.hh"
#include "util/logger.hh"

using namespace dibibase::lang::cql3;
using namespace antlr4;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <CQL file path>" << std::endl;
    exit(EXIT_SUCCESS);
  }

  std::ifstream stream;
  stream.open(argv[1]);

  ANTLRInputStream input(stream);
  CqlLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  // tokens.fill();
  // for (auto token : tokens.getTokens()) {
  //   std::cout << token->toString() << std::endl;
  // }

  CqlParser parser(&tokens);

  // std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

  InsertStatementVisitor visitor;
  auto res = visitor.visitRoot(parser.root()).as<InsertStatement>();

  Logger::make().info(res.m_table);

  return EXIT_SUCCESS;
}
