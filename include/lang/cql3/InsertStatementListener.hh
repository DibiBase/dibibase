#include <string>
#include <vector>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"
#include "CqlParserBaseListener.h"

#include "common.hh"

namespace dibibase::lang::cql3 {

class DIBIBASE_PUBLIC InsertStatementListener : public CqlParserBaseListener {
public:
  std::string m_table;
  std::vector<std::string> m_columns;
  std::vector<std::string> m_values;

  void exitInsert(CqlParser::InsertContext *ctx) override {
    auto table_ctx = ctx->table();
    { m_table = table_ctx->OBJECT_NAME()->getText(); }

    auto column_list_ctx = ctx->insertColumnSpec()->columnList();
    for (auto column_ctx : column_list_ctx->column()) {
      m_columns.push_back(column_ctx->OBJECT_NAME()->getText());
    }

    auto value_list_ctx = ctx->insertValuesSpec()->expressionList();
    for (auto column_ctx : value_list_ctx->expression()) {
      m_values.push_back(column_ctx->constant()->getText());
    }
  }
};

} // namespace dibibase::lang::cql3