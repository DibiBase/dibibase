#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>
#include <iostream>

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "db/index_page.hh"
#include "mem/summary.hh"

namespace dibibase::io {

class DIBIBASE_PUBLIC DiskManager {

public:
  static DiskManager &get_instance() {
    static DiskManager instance;
    return instance;
  }

  DiskManager(DiskManager const &) = delete;
  void operator=(DiskManager const &) = delete;

  // Loading database_path/table_name/summary_sstable_id.db into Summary by
  // using mem::Summary::from()
  std::unique_ptr<mem::Summary> load_summary(std::string &database_path,
                                             std::string &table_name,
                                             size_t sstable_id);

  // Loading a specific page from database_path/table_name/index_sstable_id.db
  // by using db::IndexPage::from()
  std::unique_ptr<db::IndexPage> load_index_page(std::string &database_path,
                                                 std::string &table_name,
                                                 size_t sstable_id,
                                                 uint8_t page_num);

  // Getting a record from database_path/table_name/data_sstable_id.db
  // indicating the record offset within the data file.
  catalog::Record get_record_from_data(std::string &database_path,
                                       std::string &table_name,
                                       size_t sstable_id, catalog::Schema &,
                                       off_t offset);

  off_t get_data_file_size(std::string &database_path,
                              std::string &table_name, size_t sstable_id);

  void remove_sstable(std::string &database_path,
                              std::string &table_name, size_t sstable_id);

private:
  DiskManager() {}
};

} // namespace dibibase::io
