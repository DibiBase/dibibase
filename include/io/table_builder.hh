#pragma once

#include "common.hh"
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
#include "db/index_page.hh"
#include "mem/summary.hh"
#include "util/buffer.hh"

namespace dibibase::io {
class DIBIBASE_PUBLIC TableBuilder {
public:
  TableBuilder(
      std::string directory_path, std::string table_name,
      catalog::Schema schema, size_t sstable_id,
      std::map<std::unique_ptr<catalog::Data>, catalog::Record> records);

  std::unique_ptr<mem::Summary> get_new_summary() {
    return std::move(m_summary);
  }

  ~TableBuilder();

private:
  // Path is: directory_path/table_name/[file-type]_new_sstable_id.db

  void fill_data_buffer();

  void fill_index_buffer(catalog::Data *record_key, off_t offset);

private:
  std::string m_directory_path;
  std::string m_table_name;
  catalog::Schema m_schema;
  size_t m_new_sstable_id;
  std::map<std::unique_ptr<catalog::Data>, catalog::Record> m_records;

  std::unique_ptr<mem::Summary> m_summary;
  std::unique_ptr<db::IndexPage> m_index_page;

  int m_file_descriptors[3];

  std::unique_ptr<util::Buffer> m_data_buffer;
  std::unique_ptr<util::Buffer> m_index_buffer;
  std::unique_ptr<util::Buffer> m_summary_buffer;
};
} // namespace dibibase::io