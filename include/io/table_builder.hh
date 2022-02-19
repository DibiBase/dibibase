#pragma once

#include "common.hh"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "db/index_page.hh"
#include "mem/summary.hh"

namespace dibibase::io {
class DIBIBASE_PUBLIC TableBuilder {
public:
  TableBuilder(std::string table_name, catalog::Schema schema,
               size_t sstable_id,
               std::map<catalog::Field, catalog::Record> records)
      : m_table_name(table_name), m_schema(schema),
        m_new_sstable_id(sstable_id + 1), m_records(records) {}

  void construct_sstable_files();

  std::unique_ptr<mem::Summary> get_new_summary() {
    return std::move(m_new_summary);
  }

private:
  // Path is: table_name/[file-type]_new_sstable_id.db

  // Looping through all record:
  // For each record, call Record::bytes() then call construct_index_file()
  // Then create new data file with the filled data buffer.
  void construct_data_file();

  // Creating an IndexPage instance then start filling it using
  // IndexPage::add_sort_key() Once IndexPage is filled, we call
  // IndexPage::bytes(), filling our index buffer and calling
  // construct_summary_file(). Then create a new IndexPage if there is more
  // record keys to store, otherwise Create new index file with the filled index
  // buffer.
  void construct_index_file();

  // Uses mem::Summary m_new_summary then start filling it using
  // Summary::push_back() then calling Summary::bytes(), filling our summary
  // buffer. Then create a new summary file with the filled buffer.
  void construct_summary_file();

private:
  std::string m_table_name;
  catalog::Schema m_schema;
  size_t m_new_sstable_id;
  std::map<catalog::Field, catalog::Record> m_records;
  std::unique_ptr<mem::Summary> m_new_summary;
};
} // namespace dibibase::io