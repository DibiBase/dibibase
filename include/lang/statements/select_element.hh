#pragma once

#include <string>

#include "common.hh"

namespace dibibase::lang {

class DIBIBASE_PUBLIC SelectElement {

public:
  SelectElement(std::string name = "", bool star = false,
                std::string alias = "")
      : m_name(name), m_star(star), m_alias(alias) {}

public:
  std::string m_name;
  bool m_star;
  std::string m_alias;
};

} // namespace dibibase::lang
