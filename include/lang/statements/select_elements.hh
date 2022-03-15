#pragma once

#include <string>
#include <vector>

#include "common.hh"
#include "lang/statements/select_element.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC SelectElements {

public:
  SelectElements(bool star = false,
                 std::vector<SelectElement> select_elements = {})
      : m_star(star), m_elements(select_elements) {}

public:
  bool m_star;
  std::vector<SelectElement> m_elements;
};

} // namespace dibibase::lang
