#include "db/index_page.hh"
#include <cstdint>
#include <iostream>
#include <memory>
#include <sys/types.h>

using namespace dibibase::db;
using namespace dibibase::catalog;

std::unique_ptr<IndexPage> IndexPage::from(util::Buffer *buff,
                                           Data::Type data_type) {
  // Creating an IndexPage from the buffer.
  std::unique_ptr<IndexPage> index_page = std::make_unique<IndexPage>();

  uint64_t actual_page_size = buff->get_int64();

  // TODO: some extra bytes are extracted which isn't part of the actual data.
  while (buff->current_offset() < actual_page_size) {
    std::unique_ptr<Data> data = Data::from(buff, data_type);
    off_t offset = buff->get_int32();

    index_page->add_sort_key(data.release(), offset);
  }

  return index_page;
}

bool IndexPage::add_sort_key(catalog::Data *key, off_t offset) {
  // Checking if the size of (key + offset) exceeds the remaining size of the
  // index page.
  size_t key_size = key->type().size();
  if (key_size + sizeof(offset) > m_size - m_current_size) {
    return false;
  }

  m_current_size += (key_size + sizeof(offset));
  m_sort_keys[key] = offset;
  return true;
}

off_t IndexPage::find_key_offset(catalog::Data *d) { return m_sort_keys[d]; }

void IndexPage::bytes(util::Buffer *buff) {
  // Storing the actual size of data.
  buff->put_int64(m_current_size);

  for (auto key : m_sort_keys) {
    // Filling buffer with data in bytes.
    key.first->bytes(buff);
    // Filling buffer with offset bytes.
    buff->put_int32(key.second);
  }
}