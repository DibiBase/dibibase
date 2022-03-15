#pragma once

#include <string>
#include <vector>

#include "common.hh"
#include "lang/statements/relation_element.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC WhereSpec {

public:
  WhereSpec(std::vector<RelationElement> relation_elements = {})
      : m_relation_elements(relation_elements) {}

public:
  std::vector<RelationElement> m_relation_elements;
};

} // namespace dibibase::lang
