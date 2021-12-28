#pragma once

#include <memory>
#include <algorithm>
#include <vector>

#include "common.hh"
#include "memory" //just as a sanity check


#include "catalog/data.hh"
#include "data.hh"
#include "util/buffer.hh"
#include "catalog/schema.hh"
namespace dibibase::catalog {

class DIBIBASE_PUBLIC Record {

public:

  class DIBIBASE_PUBLIC Id {

  public:
    Id(uint64_t page_number, off_t offset);

    static Id from(util::Buffer);

    uint64_t get_page_num() { return m_page_number; }
    off_t get_offset() { return m_offset; }

    size_t size() const;
    std::unique_ptr<char[]> bytes() const;

  private:
    uint64_t m_page_number;
    uint16_t m_offset;
  };

public:
  Record(std::vector<Data> values) : m_values(values){};
  //static Record from(util::Buffer, Schema);
  template <class... Values> static Record from(Values &&...) {}

  const std::vector<Data> values() const { return m_values; }
  const Data &operator[](std::vector<Data>::size_type idx) const {
    return m_values[idx];
  }

  Record &operator+=(const Record &rhs) {
    m_values.insert(m_values.begin(), rhs.values().begin(), rhs.values().end());
    return *this;
  }
  friend Record operator+(Record lhs, const Record &rhs) {
    lhs += rhs;
    return lhs;
  }

  size_t size() const;

  std::vector<Data> from(util::Buffer *buffer, Schema schema){
    std::vector<Data> data;
    std::unique_ptr<Data> data_ptr;
    std::vector<Field> fields = schema.fields();
    using iter = std::vector<Field>::const_iterator;
  
      for(iter it2= fields.begin() ; it2 != fields.end(); ++it2){
            //int size = it2->size();
            Data::Type type = it2->type();

            if (type.id() == Data::Type::ASCII){
                std::unique_ptr<Data> data_ptr = Data::from(buffer,type);
                data.push_back(*data_ptr);
            }
      }
    return data;
  }

  /**
  *  @brief Input is a list of Data and output is ptr to arr of bytes
  */
  std::unique_ptr<char[]> bytes(util::Buffer *buffer) {
    std::unique_ptr<char[]> bytes;
    using Iter = std::vector<Data>::const_iterator;
    Data *data = nullptr;
    for (Iter it = m_values.begin(); it != m_values.end(); ++it) {
      // check the data type
      Data::Type type = it->type();

      switch (type.id()) {
      case Data::Type::ASCII: {
        ASCIIData *ascii_data = dynamic_cast<ASCIIData *>(data);
        string s = ascii_data->data();
        buffer->put_string(s);
        break;
      }
      case Data::Type::BIGINT: {
        BigIntData *bigint_data = dynamic_cast<BigIntData *>(data);
        uint64_t bigint_d = bigint_data->data();
        buffer->put_uint64(bigint_d);
        break;
      }
      case Data::Type::BOOLEAN: {
        break;
      }
      case Data::Type::DOUBLE: {
        break;
      }
      case Data::Type::FLOAT: {
        break;
      }
      case Data::Type::INT: {
        break;
      }
      case Data::Type::SMALLINT: {
        break;
      }
      case Data::Type::TINYINT: {
        break;
      }
      }
    }

    return bytes;
  }

private:
  std::vector<Data> m_values;
};

} // namespace dibibase::catalog
