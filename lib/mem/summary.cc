#include "mem/summary.hh"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <sys/types.h>

#include "catalog/data.hh"

using namespace dibibase::mem;

std::unique_ptr<Summary> Summary::from(util::Buffer *buf) {
  auto sort_keys_length = buf->get_uint8();
  auto read_type = catalog::Data::Type::from(buf);
  auto type = catalog::Data::Type(read_type->id(), read_type->length());

  std::vector<std::unique_ptr<catalog::Data>> values;
  for (uint8_t i = 0; i < sort_keys_length; i++) {
    auto field = catalog::Data::from(buf, type);
    values.push_back(std::move(field));
  }

  return std::make_unique<Summary>(std::move(values), type);
}

uint8_t Summary::find_page_number(catalog::Data *key) {
  // In case having only one page in summary.
  if(m_sort_keys.size() == 1) return 0;

  // CHECK: Maybe the logic is wrong. (perform binary search instead)
  for (std::vector<std::unique_ptr<catalog::Data>>::size_type i = 0;
       i <= m_sort_keys.size() - 1; i++) {
    if (m_sort_keys[i]->compare(key))
      return i - 1;
  }

  return m_sort_keys.size() - 1;
}

bool Summary::push_back(catalog::Data *key) {
  if (size() + m_type.length() > 4096)
    return false;

  m_sort_keys.push_back(catalog::Data::from(key));
  return true;
}

size_t Summary::size() const {
  return sizeof(uint8_t) + m_type.size() + m_sort_keys.size() * m_type.length();
}

void Summary::bytes(util::Buffer *buf) {
  // Filling buffer with the total number of keys.
  buf->put_uint8(m_sort_keys.size());
  m_type.bytes(buf);

  for (const auto &key : m_sort_keys) {
    // Filling buffer with data in bytes.
    key->bytes(buf);
  }
}
