#pragma once

#include <cstddef>
#include <cstdint>
#include <endian.h>


#include "common.hh"

namespace dibibase::io {

class DIBIBASE_PUBLIC Endian {
public:
  Endian();

  virtual uint64_t convert_to();
  virtual ~Endian();
};

class DIBIBASE_PUBLIC LittleEndian : public Endian {
public:
  explicit LittleEndian(uint64_t data) : m_data(data) {}

  uint64_t convert_to() override {
    return htole64(m_data);
  }
private:
  uint64_t m_data;
};

class DIBIBASE_PUBLIC HostEndian : public Endian {
public:
  explicit HostEndian(uint64_t data) : m_data(data) {}

  uint64_t convert_to() override {
    return le64toh(m_data);
  }
private:
  uint64_t m_data;
};

} // namespace dibibase::util