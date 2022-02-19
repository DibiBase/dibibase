#pragma once

#include "common.hh"
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "mem/summary.hh"
#include "catalog/schema.hh"

namespace dibibase::db {
class DIBIBASE_PUBLIC TableManager {
public:
  TableManager(std::string table_name, catalog::Schema schema,
               std::vector<std::unique_ptr<mem::Summary>> summary)
      : m_table_name(table_name), m_schema(schema),
        m_current_sstable_id(summary.size() - 1),
        m_summary(std::move(summary)) {}

  // Check if memtable has enough space to store new record, otherwise flush the
  // memtable into disk and clear the memtable then store the new one. Check the
  // given record aganist schema, then get the sort key from the schema then
  // insert the new record in the memtable.
  void write_record(catalog::Record);

  // It checks if memtable contains the record, otherwise it gets the page
  // number which contains the record key from summay, this is done by calling
  // summary::find_index_page() then it gets the index page from io::DiskManager
  // and perform binary search on this IndexPage so that it gets the exact
  // offset within the data file. Finally it gets the exact record from
  // io::DiskManager and returns it.
  catalog::Record read_Record(catalog::Field);

private:
  bool find_record_in_memtable(catalog::Field);

  // Used when record exists in memtable.
  catalog::Record read_record_from_memtable(catalog::Field);

  // This function calls io::DiskManager::load_index_page() to load index page
  // then calling db::IndexPage::find_key_offset().
  // Returns the record offset within the data file.
  off_t find_record_key_in_index_page(catalog::Field);
  
  // Flushing memtable into disk by creating an instance of io::TableBuilder
  // then calling TableBuilder::construct_sstable_files(), then getting the new
  // created summary and update our vector, and sstable_id.
  void flush();

  void clear_memtable();

private:
  std::string m_table_name;
  catalog::Schema m_schema;
  size_t m_current_sstable_id;
  std::map<catalog::Field, catalog::Record> m_memtable;

  // Storing summary for sort key offsets for each sstable, in which
  // the index represents the sstable_id.
  std::vector<std::unique_ptr<mem::Summary>> m_summary;

  // TO DO: adding a vector of bloom filters.
};
} // namespace dibibase::db