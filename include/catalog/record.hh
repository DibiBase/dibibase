#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Record {
public:
  Record(std::vector<std::shared_ptr<catalog::Data>> values)
      : m_values(std::move(values)), deleted(std::make_shared<catalog::BooleanData>(false)){};

  static std::unique_ptr<Record> from(util::Buffer *, Schema);

  const std::vector<std::shared_ptr<catalog::Data>> &values() const {
    return m_values;
  }

  const std::shared_ptr<catalog::Data> &operator[](
      std::vector<std::shared_ptr<catalog::Data>>::size_type index) const {
    return m_values[index];
  }

  void bytes(util::Buffer *);

  bool is_deleted()const {
    return deleted->data() ;
  }

  void set_deleted(){
    deleted->set_data(true);
  }

private:
  std::vector<std::shared_ptr<catalog::Data>> m_values;
  std::shared_ptr<catalog::BooleanData> deleted;
};

} // namespace dibibase::catalog