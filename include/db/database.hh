#pragma once

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "table_manager.hh"

#include <dirent.h>
#include <memory>
#include <unistd.h>

namespace dibibase::db {

class DIBIBASE_PUBLIC Database {

public:
  // Database directory contains a directory for each table created and a
  // metadata file (information about each table).
  // The constructor is responsible for:
  // 1- opening a directory if it already exists or creating a directory.
  // 2- opening metadata file:
  //    In case of already existing tables:
  //    a- construct catalog::Table from it using Table::From().
  //    b- get summary file of each table from io::DiskManager.
  //    c- construct a db::TableManager for each table.
  //    Otherwise: metadata is empty then there is no tables.
  Database(std::string base_path);

  // Creating a new table in the database by creating catalog::Table instance
  // and then store its information into the metadata file using
  // catalog::Table::bytes, and add db::TableManager for it.
  void create_table(std::string table_name, catalog::Schema);

  catalog::Record read_record(std::string table_name,
                              std::unique_ptr<catalog::Data>);

  void write_record(std::string table_name, catalog::Record);

  void flush_metadata();

  ~Database() { flush_metadata(); }

private:
  std::string m_base_path;
  std::map<std::string, TableManager> m_table_managers;
};

} // namespace dibibase::db
