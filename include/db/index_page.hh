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
  IndexPage(std::map<std::unique_ptr<catalog::Data>, off_t, catalog::DataCmp>
                sort_keys,
            catalog::Data::Type type)
      : m_sort_keys(std::move(sort_keys)), m_type(type) {}

  IndexPage(catalog::Data::Type type) : m_type(type) {}

  static std::unique_ptr<IndexPage> from(util::Buffer *);

  // find the exact offset within the data file.
  off_t find_offset(std::unique_ptr<catalog::Data>);

  // Before adding a new key in the map check that the
  // page has enough space to store (key, offset) in bytes.
  bool push_back(catalog::Data *, off_t);

  const std::map<std::unique_ptr<catalog::Data>, off_t, catalog::DataCmp> &
  sort_keys() const {
    return m_sort_keys;
  }

  void clear() { m_sort_keys.clear(); }

  size_t size() const;
  void bytes(util::Buffer *);

private:
  // Mapping each sort key to its offset in the data file.
  std::map<std::unique_ptr<catalog::Data>, off_t, catalog::DataCmp> m_sort_keys;
  catalog::Data::Type m_type;
};

} // namespace dibibase::db
