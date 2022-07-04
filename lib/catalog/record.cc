#include "catalog/record.hh"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

using namespace dibibase::catalog;

std::unique_ptr<Record> Record::from(util::Buffer *buf, Schema schema) {
  std::vector<std::shared_ptr<Data>> values;
  for (std::vector<Field>::size_type i = 0; i < schema.fields().size(); i++) {
    auto field = Data::from(buf, schema[i].type());
    values.push_back(std::move(field));
  }

  return std::make_unique<Record>(std::move(values));
}

void Record::bytes(util::Buffer *buf) {
  for (auto &value : m_values)
    value->bytes(buf);
}

size_t Record::size()
{
  size_t record_size=0;
  for (auto &value : m_values)
    record_size+=value->size();
  
  return record_size;
}