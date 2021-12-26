#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unistd.h>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "catalog/table.hh"
#include "common.hh"
#include "io/manager.hh"
#include "mem/manager.hh"
#include "mem/page-replacement-strategy.hh"

namespace dibibase::db {

class DIBIBASE_PUBLIC Database {
  static constexpr const char METADATA_TABLE_NAME[17] = "_metadata.tables";

public:
  Database(
      std::string path,
      std::unique_ptr<mem::PageReplacementStrategy> page_replacement_strategy)
      : m_disk_space_manager(path),
        m_buffer_manager(m_disk_space_manager, 1024,
                         std::move(page_replacement_strategy)) {
    // TODO: open path and make sure it exists and that it's a directory
    // TODO: if METADATA_TABLE_NAME doesn't exist then:
    //  TODO: allocat the very first partition from the disk space manager and
    //  allocate the first page there
    // TODO: create meatadata table with METADATA_TABLE_NAME // Note: table
    // creation should be a non-destructive operation

    using namespace catalog;

    // --- metadata table schema ---
    // Schema()
    // .push_back({"table_name", {Data::Type::ASCII, 32}})
    // .push_back({"part_num", {Data::Type::INT,
    //  Data::Type::get_size_from_id(Data::Type::INT)}}) .add("schema",
    // .push_back(<schema>) // requires the BLOB data type needs to be
    // implemented in data.hh first. This will store the schema as bytes and
    // deserialize it using from function
  }

  ~Database() {
    // TODO: evict everything from the the buffer manager
  }

  const io::Manager &disk_space_manager() { return m_disk_space_manager; }
  const mem::Manager &buffer_manager() { return m_buffer_manager; }

private:
  catalog::Table m_metadata_table;
  io::Manager m_disk_space_manager;
  mem::Manager m_buffer_manager;
};

} // namespace dibibase::db
