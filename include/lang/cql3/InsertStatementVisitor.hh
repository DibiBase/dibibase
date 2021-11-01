#include <cstdio>
#include <string>
#include <typeinfo>
#include <vector>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"
#include "CqlParserBaseVisitor.h"

#include "common.hh"
#include "lang/cql3/statements/InsertStatement.hh"
#include "util/logger.hh"

using namespace dibibase::util;

namespace dibibase::lang::cql3 {

class DIBIBASE_PUBLIC InsertStatementVisitor : public CqlParserBaseVisitor {
public:
  antlrcpp::Any visitRoot(CqlParser::RootContext *ctx) override {
    auto values = visitCqls(ctx->cqls());
    if (values.isNotNull()) {
      try {
        return values;
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }
    return nullptr;
  }

  antlrcpp::Any visitCqls(CqlParser::CqlsContext *ctx) override {
    auto values = visitCql(ctx->cql()[0]);
    if (values.isNotNull()) {
      try {
        return values;
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }
    return nullptr;
  }

  antlrcpp::Any visitCql(CqlParser::CqlContext *ctx) override {
    auto values = visitInsert(ctx->insert());
    if (values.isNotNull()) {
      try {
        return values;
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }
    return nullptr;
  }

  antlrcpp::Any visitInsert(CqlParser::InsertContext *ctx) override {
    InsertStatement insertStatement;

    auto table_name = visitTable(ctx->table());
    if (table_name.isNotNull()) {
      try {
        insertStatement.m_table = table_name.as<std::string>();
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }

    auto columns = visitInsertColumnSpec(ctx->insertColumnSpec());
    if (columns.isNotNull()) {
      try {
        insertStatement.m_columns = columns.as<std::vector<std::string>>();
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }

    auto values = visitInsertValuesSpec(ctx->insertValuesSpec());
    if (values.isNotNull()) {
      try {
        insertStatement.m_values = values.as<std::vector<std::string>>();
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }

    return insertStatement;
  }

  antlrcpp::Any visitTable(CqlParser::TableContext *ctx) override {
    return ctx->OBJECT_NAME()->getText();
  }

  antlrcpp::Any visitColumnList(CqlParser::ColumnListContext *ctx) override {
    std::vector<std::string> columns;

    for (auto column_ctx : ctx->column()) {
      auto column = visitColumn(column_ctx);
      if (column.isNotNull()) {
        try {
          columns.push_back(column.as<std::string>());
        } catch (std::bad_cast &ex) {
          Logger::make().err(ex.what());
        }
      }
    }

    return columns;
  }

  antlrcpp::Any visitColumn(CqlParser::ColumnContext *ctx) override {
    return ctx->OBJECT_NAME()->getText();
  }

  antlrcpp::Any
  visitInsertValuesSpec(CqlParser::InsertValuesSpecContext *ctx) override {
    auto values = visitExpressionList(ctx->expressionList());
    if (values.isNotNull()) {
      try {
        return values;
      } catch (std::bad_cast &ex) {
        Logger::make().err(ex.what());
      }
    }

    return nullptr;
  }

  antlrcpp::Any
  visitExpressionList(CqlParser::ExpressionListContext *ctx) override {
    std::vector<std::string> values;

    for (auto expression_ctx : ctx->expression()) {
      auto value = visitExpression(expression_ctx);
      if (value.isNotNull()) {
        try {
          values.push_back(value.as<std::string>());
        } catch (std::bad_cast &ex) {
          Logger::make().err(ex.what());
        }
      }
    }

    return values;
  }

  antlrcpp::Any visitExpression(CqlParser::ExpressionContext *ctx) override {
    return visitConstant(ctx->constant());
  }

  antlrcpp::Any visitConstant(CqlParser::ConstantContext *ctx) override {
    return ctx->getText();
  }
};

} // namespace dibibase::lang::cql3