#pragma once

#include <cstddef>
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "catalog/table.hh"
#include "common.hh"
#include "util/logger.hh"

namespace dibibase::db {

class DIBIBASE_PUBLIC KeyValueStore {

public:
  virtual ~KeyValueStore() {}

  virtual catalog::Data read(catalog::Data *key_vlaue) = 0;
  virtual void write(const catalog::Record values) = 0;
};

class DIBIBASE_PUBLIC SSTablesKeyValueStore : public KeyValueStore {

public:
  SSTablesKeyValueStore(std::string table_name, catalog::Schema schema,
                        size_t current_sstable)
      : m_table(catalog::Table(table_name, schema, current_sstable)) {}

  catalog::Data read(catalog::Data *key_vlaue) override;
  void write(const catalog::Record values) override;

private:
  void flush();

private:
  catalog::Table m_table;
  std::multimap<std::string, std::unique_ptr<catalog::Data>> m_memtable;
};

} // namespace dibibase::db
