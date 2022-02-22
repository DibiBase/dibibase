#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/data.hh"

namespace dibibase::db {

/*
 * IndexPage represents the basic unit of data stored in the index file.
 * It has a fixed size (4096 bytes)
 * Each page stores multiple sort keys with their offsets in data file.
 */
class DIBIBASE_PUBLIC IndexPage {
public:
  IndexPage() : m_size(4096) {}

  static std::unique_ptr<IndexPage> from(util::Buffer *);

  // Before adding a new key in the map check that the
  // page has enough space to store (key, offset) in bytes.
  bool &add_sort_key(catalog::Data*, off_t);

  // find the exact offset within the data file.
  off_t find_key_offset(catalog::Data);

  void bytes(util::Buffer *) const;

private:
  uint16_t m_size;

  // Mapping each sort key to its offset in the data file.
  std::map<catalog::Data*, off_t> m_sort_keys;

  // TO DO: add padding so that the whole size of a page reach 4096 byte.
};
} // namespace dibibase::db