#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "common.hh"
#include "db/sstable_builder.hh"
#include "db/sstable_wrapper.hh"

#define MAX_MEMTABLE_SIZE 4

namespace dibibase::db {

class DIBIBASE_PUBLIC DB {

public:
  virtual void write(std::string, std::string) = 0;
  virtual std::string read(std::string) = 0;
  virtual void remove(std::string) = 0;

  virtual ~DB() {}
};

class DIBIBASE_PUBLIC DBImpl : public DB {

public:
  explicit DBImpl(){};
  void write(std::string key, std::string value) override;
  std::string read(std::string key) override;
  void remove(std::string key) override;
  void flush();
  std::size_t get_memtable_size();
  void clear_memtable();

  virtual ~DBImpl() override {}

private:
  std::multimap<std::string, std::string> m_memtable;
  //std::vector<std::unique_ptr<SSTableBuilder>> m_sstables;
};

} // namespace dibibase::db
