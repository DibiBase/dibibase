#include "db/database.hh"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "catalog/table.hh"
#include "db/table_manager.hh"
#include "io/disk_manager.hh"
#include "mem/summary.hh"
#include "util/logger.hh"

using namespace dibibase::db;
using namespace dibibase::catalog;
using namespace dibibase::util;
using namespace dibibase::mem;

Database::Database(std::string base_path) : m_base_path(base_path) {
  DIR *dir = opendir(base_path.c_str());
  if (!dir)
    mkdir(base_path.c_str(), 0777);
  closedir(dir);

  int fd = open((base_path + "/metadata.meta").c_str(), O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  uint32_t length = 0;
  int rc = read(fd, &length, sizeof(uint32_t));
  if (rc != sizeof(uint32_t) || length == 0)
    return;

  std::unique_ptr<unsigned char[]> buf =
      std::unique_ptr<unsigned char[]>(new unsigned char[length]);
  rc = read(fd, buf.get(), length);
  close(fd);

  util::Buffer *metadata_buffer =
      new util::MemoryBuffer(std::move(buf), length);

  while (metadata_buffer->offset() < length) {
    auto table = catalog::Table::from(metadata_buffer);
    auto table_name = std::string(table->name());

    std::vector<std::unique_ptr<mem::Summary>> summaries;

    for (size_t i = 0; i < table->last_sstable_id(); i++) {
      auto summary = io::DiskManager::get_instance().load_summary(
          base_path, table_name, i);
      summaries.push_back(std::move(summary));
    }

    auto table_manager = db::TableManager(
        base_path, table->name(),
        std::make_unique<catalog::Schema>(table->schema().sort_key_index(),
                                          table->schema().partition_key_index(),
                                          table->schema().fields()),
        std::move(summaries));

    m_table_managers.insert({table_name, std::move(table_manager)});
  }

  delete metadata_buffer;
}

void Database::create_table(std::string table_name, catalog::Schema schema) {
  mkdir((m_base_path + "/" + table_name).c_str(), 0777);

  auto table_manager =
      db::TableManager(m_base_path, table_name,
                       std::make_unique<catalog::Schema>(
                           schema.sort_key_index(),
                           schema.partition_key_index(), schema.fields()),
                       {});

  m_table_managers.insert({table_name, std::move(table_manager)});
}

Record Database::read_record(std::string table_name,
                             std::unique_ptr<catalog::Data> data) {
  auto table_managers_it = m_table_managers.find(table_name);

  if (table_managers_it != m_table_managers.end())
    return table_managers_it->second.read_record(std::move(data));

  throw uncomparable_type_error("");
}

void Database::write_record(std::string table_name, catalog::Record record) {
  auto table_managers_it = m_table_managers.find(table_name);

    catalog::BooleanData deleted(false);
    std::vector<std::shared_ptr<catalog::Data>> x=record.values();
    x.push_back(std::make_shared<catalog::BooleanData>(deleted));

  if (table_managers_it != m_table_managers.end())
    table_managers_it->second.write_record(record);
}

void Database::delete_record(std::string table_name, catalog::Record record) {
  auto table_managers_it = m_table_managers.find(table_name);

  catalog::BooleanData deleted(true);
    std::vector<std::shared_ptr<catalog::Data>> x=record.values();
    x.push_back(std::make_shared<catalog::BooleanData>(deleted));

  if (table_managers_it != m_table_managers.end())
    table_managers_it->second.write_record(record);
}

void Database::flush_metadata() {
  std::vector<catalog::Table> tables;

  for (auto &[key, val] : m_table_managers) {
    tables.push_back(
        catalog::Table(key,
                       catalog::Schema(val.schema()->sort_key_index(),
                                       val.schema()->partition_key_index(),
                                       val.schema()->fields()),
                       val.current_sstable_id()));
  }

  size_t length = 0;
  for (auto &table : tables)
    length += table.size();

  util::Buffer *metadata_buffer = new util::MemoryBuffer(length + 4);
  metadata_buffer->put_uint32(length);

  for (auto &table : tables) {
    table.bytes(metadata_buffer);
  }

  int fd = open((m_base_path + "/metadata.meta").c_str(), O_WRONLY);
  int rc = write(fd, metadata_buffer->bytes().get(), length);
  close(fd);

  if (rc != static_cast<int>(length))
    util::Logger::make().err("Error writing Metadata: %d", errno);

  delete metadata_buffer;
}
