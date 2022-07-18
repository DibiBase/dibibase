#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "db/index_page.hh"
#include "errors.hh"
#include "io/disk_manager.hh"
#include "io/table_builder.hh"
#include "mem/summary.hh"

namespace dibibase::db {

DEFINE_ERROR(schema_mismatch_error);
DEFINE_ERROR(non_existent_record_error);

class DIBIBASE_PUBLIC TableManager {

public:
  TableManager(std::string base_path, std::string table_name,
               std::unique_ptr<catalog::Schema> schema,
               std::vector<std::unique_ptr<mem::Summary>> summary);

  // It checks if memtable contains the record, otherwise it gets the page
  // number which contains the record key from summay, this is done by calling
  // summary::find_index_page() then it gets the index page from io::DiskManager
  // and perform binary search on this IndexPage so that it gets the exact
  // offset within the data file. Finally it gets the exact record from
  // io::DiskManager and returns it.
  catalog::Record read_record(std::unique_ptr<catalog::Data>);

  // Check if memtable has enough space to store new record, otherwise flush the
  // memtable into disk and clear the memtable then store the new one. Check the
  // given record aganist schema, then get the sort key from the schema then
  // insert the new record in the memtable.
  void write_record(catalog::Record);

  const std::unique_ptr<catalog::Schema> &schema() { return m_schema; }
  size_t current_sstable_id() { return m_current_sstable_id; }
private:
  // Flushing memtable into disk by creating an instance of io::TableBuilder
  // then calling TableBuilder::construct_sstable_files(), then getting the new
  // created summary and update our vector, and sstable_id.
  void flush();

private:
  std::string m_base_path;
  std::string m_table_name;
  std::unique_ptr<catalog::Schema> m_schema;

  size_t m_current_sstable_id;

  // TODO: adding a vector of bloom filters.

  // Storing summary for sort key offsets for each sstable, in which
  // the index represents the sstable_id.
  std::vector<std::unique_ptr<mem::Summary>> m_summaries;

  std::map<std::shared_ptr<catalog::Data>, catalog::Record, catalog::DataCmp>
      m_memtable;
};

} // namespace dibibase::db
