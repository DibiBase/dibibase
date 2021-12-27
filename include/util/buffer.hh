#pragma once

#include <cstddef>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <memory>

#include "common.hh"

namespace dibibase::util {

class DIBIBASE_PUBLIC Buffer {

public:
  Buffer() {};
  virtual ~Buffer() {};

  virtual uint64_t get_uint64() = 0;
  virtual Buffer &put_uint64(uint64_t) = 0;

  virtual uint32_t get_uint32() = 0;
  virtual Buffer &put_uint32(uint32_t) = 0;

  virtual uint16_t get_uint16() = 0;
  virtual Buffer &put_uint16(uint16_t) = 0;

  virtual uint8_t get_uint8() = 0;
  virtual Buffer &put_uint8(uint8_t) = 0;

  virtual int64_t get_int64() = 0;
  virtual Buffer &put_int64(int64_t) = 0;

  virtual int32_t get_int32() = 0;
  virtual Buffer &put_int32(int32_t) = 0;

  virtual int16_t get_int16() = 0;
  virtual Buffer &put_int16(int16_t) = 0;

  virtual int8_t get_int8() = 0;
  virtual Buffer &put_int8(int8_t) = 0;

  virtual std::string get_string(int size) = 0;
  virtual Buffer &put_string(std::string) = 0;

  virtual double get_double() = 0;
  virtual Buffer &put_double(double) = 0;

  virtual float get_float() = 0;
  virtual Buffer &put_float(float) = 0;

  virtual bool get_boolean() = 0;
  virtual Buffer &put_boolean(bool) = 0;

  virtual size_t size() const = 0;
  virtual const std::unique_ptr<unsigned char[]> bytes() const = 0;
};

class DIBIBASE_PUBLIC MemoryBuffer : public Buffer {

public:
  explicit MemoryBuffer(size_t size);
  explicit MemoryBuffer(std::unique_ptr<unsigned char[]> buf, size_t size);

  uint64_t get_uint64() override;

  Buffer &put_uint64(uint64_t data) override;

  uint32_t get_uint32() override;
  Buffer &put_uint32(uint32_t data) override;

  uint16_t get_uint16() override;
  Buffer &put_uint16(uint16_t data) override;

  uint8_t get_uint8() override;
  Buffer &put_uint8(uint8_t data) override;

  int64_t get_int64() override;
  Buffer &put_int64(int64_t) override;

  int32_t get_int32() override;
  Buffer &put_int32(int32_t) override;

  int16_t get_int16() override;
  Buffer &put_int16(int16_t) override;

  int8_t get_int8() override;
  Buffer &put_int8(int8_t) override;

  std::string get_string(int size) override;
  Buffer &put_string(std::string) override;

  double get_double() override;
  Buffer &put_double(double) override;

  float get_float() override;
  Buffer &put_float(float) override;

  bool get_boolean() override;
  Buffer &put_boolean(bool data) override;

  virtual size_t size() const override { return m_size; };
  const std::unique_ptr<unsigned char[]> bytes() const override;

  virtual ~MemoryBuffer() override {}

private:
  void advance(size_t step) { m_offset += step; }
  bool is_overflow(size_t size) {
    if(size + m_offset > m_size) return true;
    return false;
  }

private:
  size_t m_size, m_offset;
  std::unique_ptr<unsigned char[]> m_buf;
};
} // namespace dibibase::util
