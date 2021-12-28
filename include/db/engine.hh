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

class DIBIBASE_PUBLIC Engine {
  static constexpr const char METADATA_TABLE_NAME[17] = "_metadata.tables";

public:
  Engine(std::string path) : m_path(path) {
    util::Logger logger = util::Logger::make();

    m_fd = open(path.c_str(),
                O_RDONLY | O_CREAT | O_DIRECTORY | O_PATH | O_CLOEXEC);
    if (m_fd < 0)
      logger.err("Error opening directory");
    m_dir = fdopendir(m_fd);
    if (!m_dir)
      logger.err("Error opening directory");

    // TODO: Read METADATA_TABLE_NAME to populate
  }

  ~Engine() {
    close(m_fd);
    closedir(m_dir);
  }

  catalog::Data read(std::string table_name, catalog::Data *key_vlaue) {
    return store(table_name)->read(key_vlaue);
  }

  void write(std::string table_name, const catalog::Record values) {
    return store(table_name)->write(values);
  }

private:
  std::unique_ptr<db::KeyValueStore> const &
  store(std::string table_name) const {
    util::Logger logger = util::Logger::make();

    auto store_it = m_stores.find(table_name);
    if (store_it == m_stores.end()) {
      logger.err("Couldn't find store for table %s", table_name.c_str());
      exit(1);
    }

    return store_it->second;
  }

private:
  std::string m_path;
  int m_fd;
  DIR *m_dir;
  std::unordered_map<std::string, std::unique_ptr<db::KeyValueStore>> m_stores;
};

} // namespace dibibase::db
