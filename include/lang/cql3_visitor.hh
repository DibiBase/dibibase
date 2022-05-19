#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"
#include "CqlParserBaseVisitor.h"

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "errors.hh"
#include "lang/statements/batch.hh"
#include "lang/statements/create_table_statement.hh"
#include "lang/statements/drop_table_statement.hh"
#include "lang/statements/from_spec.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/order_spec.hh"
#include "lang/statements/relation_element.hh"
#include "lang/statements/select_element.hh"
#include "lang/statements/select_elements.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"
#include "lang/statements/timestamp_spec.hh"
#include "lang/statements/ttl_timestamp.hh"
#include "lang/statements/where_sepc.hh"

namespace dibibase::lang::cql3 {

DEFINE_ERROR(unknown_cql_statement);
DEFINE_ERROR(unspported_cql_statement);

class DIBIBASE_PUBLIC CQL3Visitor : public CqlParserBaseVisitor {

public:
  antlrcpp::Any visitRoot(CqlParser::RootContext *ctx) override {
    if (ctx->cqls())
      return visitCqls(ctx->cqls());
    else
      return std::vector<Statement *>();
  }

  antlrcpp::Any visitCqls(CqlParser::CqlsContext *ctx) override {
    std::vector<Statement *> statements;

    for (auto cql_ctx : ctx->cql())
      statements.push_back(visitCql(cql_ctx).as<Statement *>());

    return statements;
  }

  antlrcpp::Any visitCql(CqlParser::CqlContext *ctx) override {
    if (ctx->alterKeyspace()) {
      return visit(ctx->alterKeyspace());
    } else if (ctx->alterMaterializedView()) {
      return visit(ctx->alterMaterializedView());
    } else if (ctx->alterRole()) {
      return visit(ctx->alterRole());
    } else if (ctx->alterTable()) {
      return visit(ctx->alterTable());
    } else if (ctx->alterType()) {
      return visit(ctx->alterType());
    } else if (ctx->alterUser()) {
      return visit(ctx->alterUser());
    } else if (ctx->applyBatch()) {
      return visit(ctx->applyBatch());
    } else if (ctx->createAggregate()) {
      return visit(ctx->createAggregate());
    } else if (ctx->createFunction()) {
      return visit(ctx->createFunction());
    } else if (ctx->createIndex()) {
      return visit(ctx->createIndex());
    } else if (ctx->createKeyspace()) {
      return visit(ctx->createKeyspace());
    } else if (ctx->createMaterializedView()) {
      return visit(ctx->createMaterializedView());
    } else if (ctx->createRole()) {
      return visit(ctx->createRole());
    } else if (ctx->createTable()) {
      return visit(ctx->createTable());
    } else if (ctx->createTrigger()) {
      return visit(ctx->createTrigger());
    } else if (ctx->createType()) {
      return visit(ctx->createType());
    } else if (ctx->createUser()) {
      return visit(ctx->createUser());
    } else if (ctx->delete_()) {
      return visit(ctx->delete_());
    } else if (ctx->dropAggregate()) {
      return visit(ctx->dropAggregate());
    } else if (ctx->dropFunction()) {
      return visit(ctx->dropFunction());
    } else if (ctx->dropIndex()) {
      return visit(ctx->dropIndex());
    } else if (ctx->dropKeyspace()) {
      return visit(ctx->dropKeyspace());
    } else if (ctx->dropMaterializedView()) {
      return visit(ctx->dropMaterializedView());
    } else if (ctx->dropRole()) {
      return visit(ctx->dropRole());
    } else if (ctx->dropTable()) {
      return visit(ctx->dropTable());
    } else if (ctx->dropTrigger()) {
      return visit(ctx->dropTrigger());
    } else if (ctx->dropType()) {
      return visit(ctx->dropType());
    } else if (ctx->dropUser()) {
      return visit(ctx->dropUser());
    } else if (ctx->grant()) {
      return visit(ctx->grant());
    } else if (ctx->insert()) {
      return visit(ctx->insert());
    } else if (ctx->listPermissions()) {
      return visit(ctx->listPermissions());
    } else if (ctx->listRoles()) {
      return visit(ctx->listRoles());
    } else if (ctx->revoke()) {
      return visit(ctx->revoke());
    } else if (ctx->select_()) {
      return visit(ctx->select_());
    } else if (ctx->truncate()) {
      return visit(ctx->truncate());
    } else if (ctx->update()) {
      return visit(ctx->update());
    } else if (ctx->use_()) {
      return visit(ctx->use_());
    }

    throw unknown_cql_statement("");
  }

  antlrcpp::Any visitCreateTable(CqlParser::CreateTableContext *ctx) override {
    CreateTableStatement *create_table_statement = new CreateTableStatement();

    if (ctx->ifNotExist())
      create_table_statement->m_if_not_exists = true;
    else
      create_table_statement->m_if_not_exists = false;

    if (ctx->keyspace())
      create_table_statement->m_keyspace =
          ctx->keyspace()->OBJECT_NAME()->getText();
    else
      create_table_statement->m_keyspace = "";

    create_table_statement->m_table = ctx->table()->OBJECT_NAME()->getText();

    create_table_statement->m_schema =
        visitColumnDefinitionList(ctx->columnDefinitionList())
            .as<catalog::Schema>();

    return dynamic_cast<Statement *>(create_table_statement);
  }

  antlrcpp::Any visitColumnDefinitionList(
      CqlParser::ColumnDefinitionListContext *ctx) override {
    catalog::Schema schema = catalog::Schema(0, 0);

    size_t i = 0;
    for (auto column_definition_ctx : ctx->columnDefinition()) {
      auto res = visitColumnDefinition(column_definition_ctx)
                     .as<std::pair<catalog::Field, bool>>();
      schema.push_back(res.first);

      if (res.second) {
        schema.set_partition_key_index(i);
        schema.set_sort_key_index(i);
      }

      i++;
    }

    if (ctx->primaryKeyElement()) {
      auto [partition_column, cluster_column] =
          visitPrimaryKeyDefinition(
              ctx->primaryKeyElement()->primaryKeyDefinition())
              .as<std::pair<std::string, std::string>>();

      for (size_t i = 0; i < schema.fields().size(); i++) {
        if (schema[i].name() == partition_column)
          schema.set_partition_key_index(i);

        if (schema[i].name() == cluster_column)
          schema.set_sort_key_index(i);
      }
    }

    return schema;
  }

  antlrcpp::Any
  visitColumnDefinition(CqlParser::ColumnDefinitionContext *ctx) override {
    auto column = ctx->column()->OBJECT_NAME()->getText();
    bool primary = ctx->primaryKeyColumn() != nullptr;

    auto data_type = visitDataType(ctx->dataType()).as<catalog::Data::Type>();

    return std::pair<catalog::Field, bool>(catalog::Field(column, data_type),
                                           primary);
  }

  antlrcpp::Any visitDataType(CqlParser::DataTypeContext *ctx) override {
    return visitDataTypeName(ctx->dataTypeName());
  }

  antlrcpp::Any
  visitDataTypeName(CqlParser::DataTypeNameContext *ctx) override {
    if (ctx->K_ASCII()) {
      return catalog::Data::Type(
          catalog::Data::Type::ASCII,
          -1 // flag
          );
    } else if (ctx->K_BIGINT()) {
      return catalog::Data::Type(catalog::Data::Type::BIGINT, sizeof(int64_t));
    } else if (ctx->K_BOOLEAN()) {
      return catalog::Data::Type(catalog::Data::Type::BOOLEAN, sizeof(bool));
    } else if (ctx->K_DOUBLE()) {
      return catalog::Data::Type(catalog::Data::Type::DOUBLE, sizeof(double));
    } else if (ctx->K_FLOAT()) {
      return catalog::Data::Type(catalog::Data::Type::FLOAT, sizeof(float));
    } else if (ctx->K_INT()) {
      return catalog::Data::Type(catalog::Data::Type::INT, sizeof(int32_t));
    } else if (ctx->K_SMALLINT()) {
      return catalog::Data::Type(catalog::Data::Type::SMALLINT,
                                 sizeof(int16_t));
    } else if (ctx->K_TINYINT()) {
      return catalog::Data::Type(catalog::Data::Type::TINYINT, sizeof(int8_t));
    } else if (ctx->K_BLOB()) {
      return catalog::Data::Type(
          catalog::Data::Type::BLOB,
          strtoll(ctx->decimalLiteral()->DECIMAL_LITERAL()->getText().c_str(),
                  nullptr, 10));
    }

    throw unspported_cql_statement("Unsupported Type");
  }

  antlrcpp::Any visitPrimaryKeyDefinition(
      CqlParser::PrimaryKeyDefinitionContext *ctx) override {
    if (ctx->singlePrimaryKey()) {
      auto column = ctx->singlePrimaryKey()->column()->OBJECT_NAME()->getText();
      return std::pair<std::string, std::string>(column, column);
    }

    if (ctx->compoundKey()) {
      auto partition_column = ctx->compoundKey()
                                  ->partitionKey()
                                  ->column()
                                  ->OBJECT_NAME()
                                  ->getText();
      auto cluster_column = ctx->compoundKey()
                                ->clusteringKeyList()
                                ->clusteringKey()[0]
                                ->column()
                                ->OBJECT_NAME()
                                ->getText();
      return std::pair<std::string, std::string>(partition_column,
                                                 cluster_column);
    }

    if (ctx->compositeKey()) {
      auto partition_column = ctx->compositeKey()
                                  ->partitionKeyList()
                                  ->partitionKey()[0]
                                  ->column()
                                  ->OBJECT_NAME()
                                  ->getText();
      auto cluster_column = ctx->compositeKey()
                                ->clusteringKeyList()
                                ->clusteringKey()[0]
                                ->column()
                                ->OBJECT_NAME()
                                ->getText();
      return std::pair<std::string, std::string>(partition_column,
                                                 cluster_column);
    }

    return std::pair<std::string, std::string>();
  }

  antlrcpp::Any visitInsert(CqlParser::InsertContext *ctx) override {
    InsertStatement *insert_statement = new InsertStatement();

    if (ctx->beginBatch())
      insert_statement->m_batch =
          visitBeginBatch(ctx->beginBatch()).as<Batch>();

    if (ctx->keyspace())
      insert_statement->m_keyspace = ctx->keyspace()->OBJECT_NAME()->getText();

    insert_statement->m_table = ctx->table()->OBJECT_NAME()->getText();

    if (ctx->insertColumnSpec())
      insert_statement->m_columns =
          visitInsertColumnSpec(ctx->insertColumnSpec())
              .as<std::vector<std::string>>();

    insert_statement->m_values = visitInsertValuesSpec(ctx->insertValuesSpec())
                                     .as<std::vector<catalog::Data *>>();

    insert_statement->m_if_not_exists = ctx->ifNotExist() != nullptr;

    insert_statement->m_ttl_timestamp =
        visitUsingTtlTimestamp(ctx->usingTtlTimestamp()).as<TtlTimestamp>();

    return dynamic_cast<Statement *>(insert_statement);
  }

  antlrcpp::Any visitBeginBatch(CqlParser::BeginBatchContext *ctx) override {
    Batch batch;
    batch.m_valid = true;

    if (ctx->batchType()) {
      if (ctx->batchType()->kwLogged())
        batch.m_type = Batch::Type::LOGGED;
      else if (ctx->batchType()->kwUnlogged())
        batch.m_type = Batch::Type::UNLOGGED;
    }

    if (ctx->usingTimestampSpec()) {
      batch.m_timestamp_spec = TimestampSpec(strtoll(ctx->usingTimestampSpec()
                                                         ->timestamp()
                                                         ->decimalLiteral()
                                                         ->DECIMAL_LITERAL()
                                                         ->getText()
                                                         .c_str(),
                                                     nullptr, 10));
    }

    return batch;
  }

  antlrcpp::Any
  visitInsertColumnSpec(CqlParser::InsertColumnSpecContext *ctx) override {
    return visitColumnList(ctx->columnList());
  }

  antlrcpp::Any visitColumnList(CqlParser::ColumnListContext *ctx) override {
    std::vector<std::string> columns;

    for (auto column_ctx : ctx->column())
      columns.push_back(column_ctx->OBJECT_NAME()->getText());

    return columns;
  }

  antlrcpp::Any
  visitInsertValuesSpec(CqlParser::InsertValuesSpecContext *ctx) override {
    if (ctx->expressionList())
      return visitExpressionList(ctx->expressionList());

    throw unspported_cql_statement("JSON is Unsupported");
  }

  antlrcpp::Any
  visitExpressionList(CqlParser::ExpressionListContext *ctx) override {
    std::vector<catalog::Data *> values;

    for (auto expression_ctx : ctx->expression())
      values.push_back(visitExpression(expression_ctx).as<catalog::Data *>());

    return values;
  }

  antlrcpp::Any visitExpression(CqlParser::ExpressionContext *ctx) override {
    if (ctx->constant())
      return visitConstant(ctx->constant());

    throw unspported_cql_statement(
        "Only Constants are Supported at the moment");
  }

  antlrcpp::Any visitConstant(CqlParser::ConstantContext *ctx) override {
    catalog::Data *constant = nullptr;

    if (ctx->stringLiteral()) {
      constant = new catalog::ASCIIData(
          ctx->stringLiteral()->STRING_LITERAL()->getText());
      return constant;
    } else if (ctx->decimalLiteral()) {
      constant = new catalog::IntData(
          strtoll(ctx->decimalLiteral()->DECIMAL_LITERAL()->getText().c_str(),
                  nullptr, 10));
      return constant;
    } else if (ctx->floatLiteral()) {
      constant = new catalog::DoubleData(strtold(
          ctx->floatLiteral()->FLOAT_LITERAL()->getText().c_str(), nullptr));
      return constant;
    } else if (ctx->booleanLiteral()) {
      constant =
          new catalog::BooleanData(ctx->booleanLiteral()->K_TRUE() != nullptr);
      return constant;
    }

    throw unspported_cql_statement("Unsupported Data Type");
  }

  antlrcpp::Any
  visitUsingTtlTimestamp(CqlParser::UsingTtlTimestampContext *ctx) override {
    TtlTimestamp ttl_timestamp;

    if (ctx->ttl()) {
      ttl_timestamp.m_ttl_valid = true;
      ttl_timestamp.m_ttl = strtoll(
          ctx->ttl()->decimalLiteral()->DECIMAL_LITERAL()->getText().c_str(),
          nullptr, 10);
    }

    if (ctx->timestamp()) {
      ttl_timestamp.m_timestamp_valid = true;
      ttl_timestamp.m_ttl = strtoll(ctx->timestamp()
                                        ->decimalLiteral()
                                        ->DECIMAL_LITERAL()
                                        ->getText()
                                        .c_str(),
                                    nullptr, 10);
    }

    return ttl_timestamp;
  }

  antlrcpp::Any visitSelect_(CqlParser::Select_Context *ctx) override {
    SelectStatement *select_statement = new SelectStatement();

    if (ctx->distinctSpec())
      select_statement->m_distinct = true;

    if (ctx->kwJson())
      select_statement->m_json = true;

    select_statement->m_select_elements =
        visitSelectElements(ctx->selectElements()).as<SelectElements>();

    select_statement->m_from_spec =
        visitFromSpec(ctx->fromSpec()).as<FromSpec>();

    if (ctx->whereSpec())
      select_statement->m_where_spec =
          visitWhereSpec(ctx->whereSpec()).as<WhereSpec>();

    if (ctx->orderSpec())
      select_statement->m_order_sepc =
          visitOrderSpec(ctx->orderSpec()).as<OrderSpec>();

    if (ctx->limitSpec())
      select_statement->m_limit = strtoll(
          ctx->limitSpec()->decimalLiteral()->getText().c_str(), nullptr, 10);

    if (ctx->allowFilteringSpec())
      select_statement->m_allow_filtering = true;

    return dynamic_cast<Statement *>(select_statement);
  }

  antlrcpp::Any
  visitSelectElements(CqlParser::SelectElementsContext *ctx) override {
    SelectElements select_elements;

    select_elements.m_star = ctx->STAR() != nullptr;
    for (auto element_ctx : ctx->selectElement())
      select_elements.m_elements.push_back(
          visitSelectElement(element_ctx).as<SelectElement>());

    return select_elements;
  }

  antlrcpp::Any
  visitSelectElement(CqlParser::SelectElementContext *ctx) override {
    SelectElement select_element;

    select_element.m_name = ctx->OBJECT_NAME(0)->getText();
    select_element.m_star = ctx->STAR() != nullptr;

    if (ctx->OBJECT_NAME(1))
      select_element.m_alias = ctx->OBJECT_NAME(1)->getText();

    return select_element;
  }

  antlrcpp::Any visitFromSpec(CqlParser::FromSpecContext *ctx) override {
    FromSpec from_spec;

    if (ctx->fromSpecElement()->OBJECT_NAME(1)) {
      from_spec.m_keyspace = ctx->fromSpecElement()->OBJECT_NAME(0)->getText();
      from_spec.m_table = ctx->fromSpecElement()->OBJECT_NAME(1)->getText();
    } else {
      from_spec.m_table = ctx->fromSpecElement()->OBJECT_NAME(0)->getText();
    }

    return from_spec;
  }

  antlrcpp::Any visitWhereSpec(CqlParser::WhereSpecContext *ctx) override {
    WhereSpec where_spec;

    for (auto relation_element_ctx : ctx->relationElements()->relationElement())
      where_spec.m_relation_elements.push_back(
          visitRelationElement(relation_element_ctx).as<RelationElement>());

    return where_spec;
  }

  antlrcpp::Any
  visitRelationElement(CqlParser::RelationElementContext *ctx) override {
    RelationElement relation_element;

    if (ctx->OBJECT_NAME(1)) {
      relation_element.m_column = ctx->OBJECT_NAME(1)->getText();
    } else {
      relation_element.m_column = ctx->OBJECT_NAME(0)->getText();
    }

    if (ctx->OPERATOR_EQ()) {
      relation_element.m_op = RelationElement::EQ;
    } else if (ctx->OPERATOR_LT()) {
      relation_element.m_op = RelationElement::LT;
    } else if (ctx->OPERATOR_GT()) {
      relation_element.m_op = RelationElement::GT;
    } else if (ctx->OPERATOR_LTE()) {
      relation_element.m_op = RelationElement::LTE;
    } else if (ctx->OPERATOR_GTE()) {
      relation_element.m_op = RelationElement::GTE;
    }

    relation_element.m_operand =
        visitConstant(ctx->constant()).as<catalog::Data *>();

    return relation_element;
  }

  antlrcpp::Any visitOrderSpec(CqlParser::OrderSpecContext *ctx) override {
    return visitOrderSpecElement(ctx->orderSpecElement());
  }

  antlrcpp::Any
  visitOrderSpecElement(CqlParser::OrderSpecElementContext *ctx) override {
    OrderSpec order_spec;

    order_spec.m_valid = true;
    order_spec.m_column = ctx->OBJECT_NAME()->getText();
    order_spec.m_ascending = ctx->kwDesc() == nullptr;

    return order_spec;
  }

  antlrcpp::Any visitDropTable(CqlParser::DropTableContext *ctx) override {
    DropTableStatement *drop_table_statement = new DropTableStatement();

    if (ctx->ifExist())
      drop_table_statement->m_if_exists = true;
    else
      drop_table_statement->m_if_exists = false;

    if (ctx->keyspace())
      drop_table_statement->m_keyspace =
          ctx->keyspace()->OBJECT_NAME()->getText();
    else
      drop_table_statement->m_keyspace = "";

    drop_table_statement->m_table = ctx->table()->OBJECT_NAME()->getText();

    return dynamic_cast<Statement *>(drop_table_statement);
  }

  antlrcpp::Any visitDelete(CqlParser::DropTableContext *ctx) override {
    DeleteStatement *delete_statement = new DeleteStatement();

    if (ctx->beginBatch())
      delete_statement->m_batch =
          visitBeginBatch(ctx->beginBatch()).as<Batch>();

    if (ctx->keyspace())
      delete_statement->m_keyspace = ctx->keyspace()->OBJECT_NAME()->getText();

    delete_statement->m_table = ctx->table()->OBJECT_NAME()->getText();

    if (ctx->insertColumnSpec())
      delete_statement->m_columns =
          visitInsertColumnSpec(ctx->insertColumnSpec())
              .as<std::vector<std::string>>();

    // Remove later
    delete_statement->m_values = visitInsertValuesSpec(ctx->insertValuesSpec())
                                     .as<std::vector<catalog::Data *>>();

    delete_statement->m_if_not_exists = ctx->ifNotExist() != nullptr;

    delete_statement->m_ttl_timestamp =
        visitUsingTtlTimestamp(ctx->usingTtlTimestamp()).as<TtlTimestamp>();

    return dynamic_cast<Statement *>(delete_statement);
  }
};

} // namespace dibibase::lang::cql3
