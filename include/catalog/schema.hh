#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Record;

class DIBIBASE_PUBLIC Field {

public:
  Field(std::string name, Data::Type type) : m_name(name), m_type(type){};

  static std::unique_ptr<Field> from(util::Buffer *);

  const std::string &name() const { return m_name; }
  const Data::Type &type() const { return m_type; }

  size_t size() const;
  void bytes(util::Buffer *);

private:
  std::string m_name;
  Data::Type m_type;
};

class DIBIBASE_PUBLIC Schema {

public:
  Schema(size_t sort_key_index, size_t partition_key_index,
         std::vector<Field> fields = {})
      : m_sort_key_index(sort_key_index),
        m_partition_key_index(partition_key_index), m_fields(fields) {}

  static std::unique_ptr<Schema> from(util::Buffer *);

  bool verify(const catalog::Record &record) const;

  Schema &push_back(Field field);

  const std::vector<Field> &fields() const { return m_fields; }
  const Field &operator[](std::vector<Field>::size_type index) const {
    return m_fields[index];
  }

  size_t sort_key_index() const { return m_sort_key_index; }
  size_t partition_key_index() const { return m_partition_key_index; }

  void set_sort_key_index(size_t sort_key_index) {
    m_sort_key_index = sort_key_index;
  }
  void set_partition_key_index(size_t sort_key_index) {
    m_sort_key_index = sort_key_index;
  }

  size_t size() const;
  void bytes(util::Buffer *);

private:
  size_t m_sort_key_index;
  size_t m_partition_key_index;
  std::vector<Field> m_fields;
};

} // namespace dibibase::catalog
