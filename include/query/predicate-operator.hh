#pragma once

#include <cstddef>

#include "common.hh"

namespace dibibase::query {

class DIBIBASE_PUBLIC PredicateOperator {
public:
  enum Predicate {
    EQUALS,
    NOT_EQUALS,
    LESS_THAN,
    LESS_THAN_EQUALS,
    GREATER_THAN,
    GREATER_THAN_EQUALS
  };

public:
  PredicateOperator(Predicate predicate);

  template <class T> bool evaluate(T a, T b) {

    template <class T> bool evaluate(T a, T b) {

    switch (m_predicate) {
      case Predicate::EQUALS:
        return a==b;
      case Predicate::NOT_EQUALS:
        return a!=b;
      case Predicate::LESS_THAN:
        return a<b;
      case Predicate::LESS_THAN_EQUALS:
        return a<=b;
      case Predicate::GREATER_THAN:
        return a>b;
      case Predicate::GREATER_THAN_EQUALS:
        return a>=b;
      default:
        return false;
    }

  }

}

private:
  Predicate m_predicate;
};

} // namespace dibibase::query
