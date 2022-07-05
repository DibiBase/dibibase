#include "catalog/schema.hh"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "catalog/record.hh"

using namespace dibibase::catalog;

std::unique_ptr<Field> Field::from(util::Buffer *buf) {
  auto name_length = buf->get_uint8();
  auto name = buf->get_string(name_length);
  auto read_type = Data::Type::from(buf);
  auto type = Data::Type(read_type->id(), read_type->length());

  return std::make_unique<Field>(name, type);
}

size_t Field::size() const {
  return sizeof(uint8_t) + m_name.length() + m_type.size();
}

void Field::bytes(util::Buffer *buf) {
  buf->put_uint8(m_name.size());
  buf->put_string(m_name);
  m_type.bytes(buf);
}

std::unique_ptr<Schema> Schema::from(util::Buffer *buf) {
  auto fields_length = buf->get_uint8();

  std::vector<Field> fields;
  for (std::vector<Field>::size_type i = 0; i < fields_length; i++) {
    auto read_field = Field::from(buf);
    fields.push_back(Field(read_field->name(), read_field->type()));
  }

  auto sort_index = buf->get_uint64();
  auto partition_index = buf->get_uint64();

  return std::make_unique<Schema>(sort_index, partition_index, fields);
}

bool Schema::verify(const catalog::Record &record) const {
  auto &values = record.values();

  if (values.size() != m_fields.size())
    return false;
  for (std::vector<std::unique_ptr<catalog::Data>>::size_type i = 0;
       i < values.size(); i++) {
    if ( values[i]->type() == m_fields[i].type()) {
      continue;
    }
    return false;
  }

  return true;
}

Schema &Schema::push_back(Field field) {
  m_fields.push_back(field);
  return *this;
}


size_t Schema::size() const {
  size_t fields_size = 0;
  for (const auto &field : m_fields)
    fields_size += field.size();

  return sizeof(uint8_t) + fields_size + sizeof(uint64_t) + sizeof(uint64_t);
}

void Schema::bytes(util::Buffer *buf) {
  buf->put_uint8(m_fields.size());
  for (std::vector<Field>::size_type i = 0; i < m_fields.size(); i++)
    m_fields[i].bytes(buf);
  buf->put_uint64(m_sort_key_index);
  buf->put_uint64(m_partition_key_index);
}
