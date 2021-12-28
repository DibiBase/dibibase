#pragma once

#include <cstddef>
#include <cstdint>
#include <dirent.h>
#include <fcntl.h>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "catalog/table.hh"
#include "common.hh"
#include "db/store.hh"
#include "util/logger.hh"

namespace dibibase::db {

class DIBIBASE_PUBLIC SSTable {

public:
  SSTable(std::string path, std::string table_name, size_t sstable_id) {
    m_fd = open((path + table_name + std::to_string(sstable_id)).c_str(),
                O_RDWR | O_CREAT);
  }

  ~SSTable() { close(m_fd); }

  catalog::Data search(catalog::Data *key_vlaue); // TODO: use binary search

  void
  persist(std::multimap<std::string, std::unique_ptr<catalog::Data>> &memtable);

private:
  int m_fd;
};

} // namespace dibibase::db
