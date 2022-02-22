#pragma once

#include "common.hh"
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/data.hh"

namespace dibibase::mem {
class DIBIBASE_PUBLIC Summary {
public:
  Summary() {}

  static std::unique_ptr<Summary> from(util::Buffer *, catalog::Data::Type);

  void push_back(catalog::Data *key) { m_sort_keys.push_back(key); }

  int8_t size() { return m_sort_keys.size(); }

  std::vector<catalog::Data *> get_summary() { return m_sort_keys; }

  // Performing binary search on record key to find the page number which
  // contains this key.
  off_t find_index_page(catalog::Data *);

  void bytes(util::Buffer *);

private:
  // Storing summary of clustering keys within an sstable in which
  // the index represents the page number where this key is located.
  std::vector<catalog::Data *> m_sort_keys;
};
} // namespace dibibase::mem