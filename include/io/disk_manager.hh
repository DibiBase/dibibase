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
class DIBIBASE_PUBLIC DiskManager {
public:
  static DiskManager *get_instance() {
    if (!m_manager)
      m_manager = new DiskManager();
    return m_manager;
  }

  // Loading database_path/table_name/summary_sstable_id.db into Summary by
  // using mem::Summary::from()
  std::unique_ptr<mem::Summary> load_summary(std::string database_path,
                                            std::string table_name,
                                            size_t sstable_id);

  // Getting a record from database_path/table_name/data_sstable_id.db
  // indicating the record offset within the data file.
  catalog::Record get_record_from_data(std::string database_path,
                                       std::string table_name,
                                       size_t sstable_id, catalog::Schema,
                                       off_t offset);

  // Loading a specific page from database_path/table_name/index_sstable_id.db
  // by using db::IndexPage::from()
  std::unique_ptr<db::IndexPage> load_index_page(std::string database_path,
                                                 std::string table_name,
                                                 size_t sstable_id,
                                                 int page_num);

private:
  static DiskManager *m_manager;

  DiskManager() {}
  ~DiskManager() { delete m_manager; }
};
} // namespace dibibase::io