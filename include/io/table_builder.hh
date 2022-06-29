#pragma once

#include <cstdint>
#include <fcntl.h>
#include <map>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "db/index_page.hh"
#include "mem/summary.hh"
#include "util/buffer.hh"

namespace dibibase::io {

class DIBIBASE_PUBLIC TableBuilder {

public:
  TableBuilder(std::string &base_path, std::string &table_name,
               catalog::Schema &schema, size_t sstable_id,
               std::multimap<std::shared_ptr<catalog::Data>, catalog::Record,
                        catalog::DataCmp> records);

  std::unique_ptr<mem::Summary> get_new_summary() {
    return std::move(m_summary);
  }

  ~TableBuilder();

private:
  void construct_sstable_files();

  bool write_index_file();
  void write_summary_file();

private:
  std::string &m_base_path;
  std::string &m_table_name;
  catalog::Schema &m_schema;
  size_t m_new_sstable_id;

  std::multimap<std::shared_ptr<catalog::Data>, catalog::Record, catalog::DataCmp>
      m_records;

  int m_summary_fd;
  int m_index_fd;
  int m_data_fd;

  std::unique_ptr<mem::Summary> m_summary;
  std::unique_ptr<db::IndexPage> m_index_page;
};

} // namespace dibibase::io
