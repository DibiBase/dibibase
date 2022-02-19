#pragma once

#include "common.hh"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/schema.hh"

namespace dibibase::mem {
class DIBIBASE_PUBLIC Summary {
public:
  Summary() {}

  static std::unique_ptr<Summary> from(util::Buffer *);

  Summary &push_back(catalog::Field key) {
    m_sort_keys.push_back(key);
    return *this;
  }

  const catalog::Field &
  operator[](std::vector<catalog::Field>::size_type index) {
    return m_sort_keys[index];
  }

  // Performing binary search on record key to find the page number which
  // contains this key.
  off_t find_index_page(catalog::Field);

  size_t size() const;

  void bytes(util::Buffer *) const;

private:
  // Storing summary of clustering keys within an sstable in which
  // the index represents the page number where this key is located.
  std::vector<catalog::Field> m_sort_keys;
};
} // namespace dibibase::mem