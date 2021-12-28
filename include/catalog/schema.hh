#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Field {

public:
  Field(std::string name, Data::Type type) : m_name(name), m_type(type){};

  const std::string &name() const { return m_name; }
  const Data::Type &type() const { return m_type; }

  size_t size() const {
    return sizeof(std::uint32_t) + m_name.length() + m_type.size();
  }

private:
  std::string m_name;
  Data::Type m_type;
};

class DIBIBASE_PUBLIC Schema {

public:
  static Schema from(util::Buffer);

  Schema &push_back(Field field) {
    m_fields.push_back(field);
    return *this;
  }

  const std::vector<Field> &fields() const { return m_fields; }
  const Field &operator[](std::vector<Field>::size_type index) const {
    return m_fields[index];
  }

  bool verify(Record record);

  size_t size() const {
    size_t size = sizeof(uint32_t);    // number of entries
    for (const auto &field : m_fields) // fields
      size += field.size();

    return size;
  };
  std::unique_ptr<char[]> bytes() const {
    std::unique_ptr<char[]> buf = std::unique_ptr<char[]>(new char[size()]);
    // TODO: fill the buffer
  };

private:
  std::vector<Field> m_fields;
  size_t m_sort_key;
  size_t m_partition_key;
};

} // namespace dibibase::catalog
