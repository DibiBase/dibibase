#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/schema.hh"
#include "common.hh"

namespace dibibase::catalog {
class DIBIBASE_PUBLIC Table {

public:
  Table(std::string name, catalog::Schema schema, size_t current_sstable);

private:
  std::string m_name;
  catalog::Schema m_schema;
  size_t m_current_sstable_id;
};

} // namespace dibibase::catalog
