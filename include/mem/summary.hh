#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <vector>

#include "catalog/data.hh"
#include "common.hh"

namespace dibibase::mem {
class DIBIBASE_PUBLIC Summary {

public:
  Summary(std::vector<std::unique_ptr<catalog::Data>> sort_keys,
          catalog::Data::Type type)
      : m_sort_keys(std::move(sort_keys)), m_type(type) {}

  static std::unique_ptr<Summary> from(util::Buffer *);

  // Performing binary search on record key to find the page number which
  // contains this key.
  uint8_t find_page_number(catalog::Data *);

  bool push_back(catalog::Data *sort_key);

  const std::vector<std::unique_ptr<catalog::Data>> &sort_keys() const {
    return m_sort_keys;
  }

  size_t size() const;
  void bytes(util::Buffer *);

private:
  // Storing summary of clustering keys within an sstable in which
  // the index represents the page number where this key is located.
  std::vector<std::unique_ptr<catalog::Data>> m_sort_keys;
  catalog::Data::Type m_type;
};

} // namespace dibibase::mem
