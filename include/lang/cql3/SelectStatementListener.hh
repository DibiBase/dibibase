#include <cstdio>
#include <string>
#include <vector>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"
#include "CqlParserBaseListener.h"

#include "common.hh"

namespace dibibase::lang::cql3 {

class DIBIBASE_PUBLIC SelectStatementListener : public CqlParserBaseListener {
public:
  bool m_distinct = false;
  std::vector<std::string> m_columns;
  std::vector<std::string> m_aliases;
  std::string m_from_table;
  std::string m_order_column;
  bool m_order_direction;
  size_t m_limit;

  void exitSelect_(CqlParser::Select_Context *ctx) override {
    m_distinct = ctx->distinctSpec() != nullptr;

    auto select_elements_ctx = ctx->selectElements();
    for (auto select_element_ctx : select_elements_ctx->selectElement()) {
      auto object_names = select_element_ctx->OBJECT_NAME();
      if (object_names.size() > 0) {
        m_columns.push_back(object_names[0]->getText());
      }
      if (object_names.size() > 1) {
        m_aliases.push_back(object_names[1]->getText());
      }
    }

    auto from_element_ctx = ctx->fromSpec()->fromSpecElement();
    {
      auto object_names = from_element_ctx->OBJECT_NAME();
      if (object_names.size() > 0) {
        m_from_table = object_names[0]->getText();
      }
    }

    auto order_element_ctx = ctx->orderSpec()->orderSpecElement();
    {
      auto object_name = order_element_ctx->OBJECT_NAME();
      m_order_column = object_name->getText();
      if (order_element_ctx->kwDesc()) {
        m_order_direction = true;
      }
    }

    auto limit_ctx = ctx->limitSpec();
    { sscanf(limit_ctx->decimalLiteral()->getText().c_str(), "%zu", &m_limit); }
  }
};

} // namespace dibibase::lang::cql3