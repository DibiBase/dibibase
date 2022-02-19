#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/schema.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {
class DIBIBASE_PUBLIC Table {

public:
  Table(std::string name, catalog::Schema schema, size_t sstable_id)
      : m_name(name), m_schema(schema), m_last_sstable_id(sstable_id) {}

  static std::unique_ptr<Table> from(util::Buffer *);

  std::string get_table_name() { return m_name; }

  catalog::Schema get_table_schema() { return m_schema; }

  size_t get_last_sstable_id() { return m_last_sstable_id; }

  void bytes(util::Buffer *) const;

private:
  std::string m_name;
  catalog::Schema m_schema;
  size_t m_last_sstable_id;
};
} // namespace dibibase::catalog