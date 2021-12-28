#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "catalog/data.hh"
#include "catalog/record.hh"
#include "common.hh"
#include "util/buffer.hh"

namespace dibibase::catalog {

class DIBIBASE_PUBLIC Field {

public:
  Field(std::string name, Data::Type type) : m_name(name), m_type(type){};

  const std::string &name() const { return m_name; }
  const Data::Type &type() const { return m_type; }

  size_t size() const {
    return sizeof(std::uint32_t) + m_name.length() + m_type.size();
  }
  void bytes(util::MemoryBuffer *buf) const {
    // Util::MemoryBuffer buf(size());
    buf->put_uint32(m_name.size());
    buf->put_string(m_name);
    m_type.bytes(buf);
    
    // return buf.bytes();
  };
private:
  std::string m_name;
  Data::Type m_type;
};

class DIBIBASE_PUBLIC Schema {

public:
  static Schema from(util::MemoryBuffer buf){
    auto sz = buf.size();
    while(sz--){
      auto nameSize = buf.get_uint32();
      auto name = buf.get_string();
      auto typePtr = Data::Type.from(&buf);
      auto type = *typePtr;
      Field f(name , type);
      m_fields.push_back(f);
    }
  }

  Schema &push_back(Field field) {
    m_fields.push_back(field);
    return *this;
  }

  const std::vector<Field> &fields() const { return m_fields; }
  const Field &operator[](std::vector<Field>::size_type index) const {
    return m_fields[index];
  }

  bool verify(Record record);

  size_t size() const {
    size_t size = sizeof(uint32_t);    // number of entries
    for (const auto &field : m_fields) // fields
      size += field.size();

    return size;
  };
  std::unique_ptr<unsigned char[]> bytes() const {
      Util::MemoryBuffer buf(size());

      for(auto &field : m_fields){
          field.bytes(&buf);
      }
      return buf.bytes();
    
  };

private:
  std::vector<Field> m_fields;
};

} // namespace dibibase::catalog
