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
  deleted->bytes(buf);  
}
