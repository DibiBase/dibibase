#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "common.hh"

namespace dibibase::util {

class DIBIBASE_PUBLIC Buffer {

public:
  Buffer();

  virtual Buffer &put_uint64(uint64_t);
  virtual uint64_t get_uint64() const;

  virtual size_t size() const = 0;
  virtual const char *bytes() const = 0;

  virtual ~Buffer();
};

class DIBIBASE_PUBLIC ByteMemoryBuffer : public Buffer {

public:
  explicit ByteMemoryBuffer(size_t size)
      : m_size(size), m_idx(0), m_buf(std::make_unique<char[]>(m_size)) {}

  Buffer &put_uint64(uint64_t data) override {
    advance(sizeof(uint64_t));
    return *this;
  }
  uint64_t get_uint64() const override;

  virtual size_t size() const override { return m_size; };
  const char *bytes() const override;

private:
  size_t m_size, m_idx;
  std::unique_ptr<char[]> m_buf;

  void advance(size_t step) { m_idx += step; }
};

} // namespace dibibase::util