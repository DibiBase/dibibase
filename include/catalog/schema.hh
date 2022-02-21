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
}

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Field {

public:
  Field(std::string name, Data::Type type) : m_name(name), m_type(type){};

  static std::unique_ptr<Field> from(util::Buffer *);

  const std::string &name() const { return m_name; }
  const Data::Type &type() const { return m_type; }

  size_t size() const {
    return sizeof(std::uint32_t) + m_name.length() + m_type.size();
  }

  void bytes(util::Buffer *) const;

private:
  std::string m_name;
  Data::Type m_type;
};

class DIBIBASE_PUBLIC Schema {

public:
  Schema(size_t sort_index, size_t partition_index)
      : m_sort_index(sort_index), m_partition_index(partition_index) {}

  static std::unique_ptr<Schema> from(util::Buffer *);

  Schema &push_back(Field field) {
    m_fields.push_back(field);
    return *this;
  }

  const std::vector<Field> &fields() const { return m_fields; }

  const Field &operator[](std::vector<Field>::size_type index) const {
    return m_fields[index];
  }
  
  const Field get_partition_key() { return m_fields[m_partition_index]; }
  const Field get_sort_key() { return m_fields[m_partition_index]; }

  size_t get_sort_index() { return m_sort_index; }
  size_t get_partition_index() { return m_partition_index; }

  // Checking the given record aganist the schema.
  bool verify(Record record);

  size_t record_size() const;

  size_t schema_size() const;

  void bytes(util::Buffer *) const;

private:
  std::vector<Field> m_fields;
  size_t m_sort_index;
  size_t m_partition_index;
};

} // namespace dibibase::catalog