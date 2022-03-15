#pragma once

#include <memory>
#include <string>

#include "catalog/data.hh"
#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC RelationElement {

public:
  enum DIBIBASE_PUBLIC Operator { EQ, LT, GT, LTE, GTE };

public:
  RelationElement(std::string column = "", Operator op = EQ,
                  catalog::Data *operand = nullptr)
      : m_column(column), m_op(op), m_operand(operand) {}

  ~RelationElement() { delete m_operand; }

  RelationElement(const RelationElement &other)
      : m_column(other.m_column), m_op(other.m_op) {
    m_operand = catalog::Data::from(other.m_operand).release();
  }

  RelationElement &operator=(const RelationElement &other) {
    if (this == &other)
      return *this;

    auto new_operand = catalog::Data::from(other.m_operand).release();
    delete m_operand;

    m_column = other.m_column;
    m_op = other.m_op;
    m_operand = new_operand;
    return *this;
  }

public:
  std::string m_column;
  Operator m_op;
  catalog::Data *m_operand;
};

} // namespace dibibase::lang
