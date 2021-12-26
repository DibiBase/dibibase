#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "io/partition.hh"
#include "mem/page.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Table {

public:
  Table(std::string name, Schema schema, uint32_t partition_num)
      : m_name(name), m_schema(schema), m_partition_num(partition_num),
        m_current_offset(0),
        m_number_of_records_per_page(PAGE_SIZE / schema.size()) {}

  std::string name() { return m_name; }
  Schema schema() { return m_schema; }
  size_t number_of_records_per_page() { return m_number_of_records_per_page; }
  uint32_t partition_number() { return m_partition_num; }
  uint32_t offset() { return m_current_offset; }

  Record record(Record::Id record_id) {
    // TODO: get page using record_id
    // TODO: search using binary search
    // TODO: read record
  }

  Record::Id push_record_back(Record record) {
    // TODO: verify record against schema
    // TODO: get the current page
    // insert_record(page, record);
  }

private:
  void insert_record(mem::Page page, Record record) {
    // TODO: write to buffer
  }

private:
  std::string m_name;
  Schema m_schema;
  uint32_t m_partition_num;
  uint32_t m_current_offset;
  size_t m_number_of_records_per_page;
};

} // namespace dibibase::catalog
