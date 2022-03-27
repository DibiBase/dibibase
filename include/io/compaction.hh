#pragma once

#include <cstdint>
#include <fcntl.h>
#include <map>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "catalog/schema.hh"
#include "io/disk_manager.hh"
#include "common.hh"

namespace dibibase::io {

class DIBIBASE_PUBLIC Compaction {
public:
  Compaction(std::string &base_path, std::string &table_name,
             catalog::Schema& schema, size_t sstable_id);

  void compact_sstables();

private:
  std::string &m_base_path;
  std::string &m_table_name;

  catalog::Schema &m_schema;

  size_t m_latest_sstable_id;
  size_t m_total_records;

  io::DiskManager &m_disk_manager;
};

} // namespace dibibase::io
