#include <cstdlib>
#include <iostream>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"

#include "lang/cql3_visitor.hh"
#include "lang/statements/create_table_statement.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"

using namespace antlr4;
using namespace dibibase;
using namespace dibibase::lang::cql3;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <CQL file path>" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::ifstream stream;
  stream.open(argv[1]);

  ANTLRInputStream input(stream);
  CqlLexer lexer(&input);
  CommonTokenStream tokens(&lexer);
  CqlParser parser(&tokens);

  CqlParser::RootContext *tree = parser.root();
  CQL3Visitor visitor;
  std::vector<lang::Statement *> statements =
      visitor.visitRoot(tree).as<std::vector<lang::Statement *>>();

  db::Database db("database");

  for (auto statement : statements) {
    switch (statement->type()) {
    case lang::Statement::Type::CREATE_TABLE:
      if (lang::CreateTableStatement *create_table_statement =
              dynamic_cast<lang::CreateTableStatement *>(statement);
          create_table_statement != nullptr) {
        create_table_statement->execute(db);
        std::cout << create_table_statement->m_table << std::endl;
      }
      break;
    case lang::Statement::Type::INSERT:
      if (lang::InsertStatement *insert_statement =
              dynamic_cast<lang::InsertStatement *>(statement);
          insert_statement != nullptr) {
        insert_statement->execute(db);
        std::cout << insert_statement->m_table << std::endl;
      }
      break;
    case lang::Statement::Type::SELECT:
      if (lang::SelectStatement *select_statement =
              dynamic_cast<lang::SelectStatement *>(statement);
          select_statement != nullptr) {
        auto result = select_statement->execute(db);
        std::cout << select_statement->m_from_spec.m_table << std::endl;
        if (result)
          for (auto value : result.value().values())
            std::cout << value->print() << std::endl;
      }
      break;
    default:
      break;
    }

    delete statement;
  }

  statements.clear();

  return EXIT_SUCCESS;
}
