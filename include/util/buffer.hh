#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

#include "common.hh"

namespace dibibase::util {

class DIBIBASE_PUBLIC Buffer {

public:
  Buffer();
  virtual ~Buffer();

  virtual uint64_t get_uint64() const = 0;
  virtual Buffer &put_uint64(uint64_t) = 0;

  virtual size_t size() const = 0;
  virtual const std::unique_ptr<char[]> bytes() const = 0;
};

class DIBIBASE_PUBLIC MemoryBuffer : public Buffer {

public:
  explicit MemoryBuffer(size_t size)
      : m_size(size), m_offset(0), m_buf(std::make_unique<char[]>(m_size)) {}

  uint64_t get_uint64() const override;
  Buffer &put_uint64(uint64_t data) override {
    constexpr auto size = sizeof(uint64_t);
    memcpy(m_buf.get() + m_offset, reinterpret_cast<char *>(&data), size);
    advance(size);
    return *this;
  }

  virtual size_t size() const override { return m_size; };
  const std::unique_ptr<char[]> bytes() const override;

private:
  void advance(size_t step) { m_offset += step; }

private:
  size_t m_size, m_offset;
  std::unique_ptr<char[]> m_buf;
};

} // namespace dibibase::util
