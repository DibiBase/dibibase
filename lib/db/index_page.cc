#include "db/index_page.hh"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/types.h>

using namespace dibibase::db;
using namespace dibibase::catalog;

std::unique_ptr<IndexPage> IndexPage::from(util::Buffer *buf) {
  auto sort_keys_length = buf->get_uint8();
  auto read_type = catalog::Data::Type::from(buf);
  auto type = catalog::Data::Type(read_type->id(), read_type->length());

  std::map<std::unique_ptr<catalog::Data>, off_t> sort_keys;
  while(sort_keys_length > 0) {
    auto key = catalog::Data::from(buf, type);
    auto offset = buf->get_uint64();
    sort_keys[std::move(key)] = offset;
  }

  return std::make_unique<IndexPage>(std::move(sort_keys), type);
}

off_t IndexPage::find_key_offset(catalog::Data *key) {
  return m_sort_keys[std::unique_ptr<catalog::Data>(key)];
}

bool IndexPage::add_sort_key(catalog::Data *key, off_t offset) {
  if (size() + m_type.length() + sizeof(offset) > 4096)
    return false;

  m_sort_keys[std::unique_ptr<catalog::Data>(key)] = offset;
  return true;
}

size_t IndexPage::size() const {
  return m_type.size() + m_sort_keys.size() * (m_type.length() + sizeof(off_t));
}

void IndexPage::bytes(util::Buffer *buf) {
  // Filling buffer with total number of keys.
  buf->put_uint8(m_sort_keys.size());
  m_type.bytes(buf);

  for (const auto &key : m_sort_keys) {
    // Filling buffer with key in bytes.
    key.first->bytes(buf);
    // Filling buffer with offset in bytes.
    buf->put_uint64(key.second);
  }
}
