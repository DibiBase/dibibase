#pragma once

#include "common.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC MurmurHash {

public:
  static uint32_t hash2_x32(uint8_t key[], uint32_t offset, uint32_t length,
                           uint32_t seed);

  static uint64_t hash2_x64(uint8_t key[], uint32_t offset, uint32_t length,
                           uint64_t seed);

  static std::unique_ptr<uint64_t[]>
  hash3_x64_128(uint8_t key[], uint32_t offset, uint32_t length, uint64_t seed);

  static std::unique_ptr<uint64_t[]> inv_hash3_x64_128(uint64_t result[]);

private:
  static uint64_t get_block(uint8_t key[], uint32_t offset, uint32_t index);

  static uint64_t rotl64(uint64_t v, uint32_t n);

  static uint64_t fmix(uint64_t k);

  static uint64_t inv_rotl64(uint64_t v, uint32_t n);

  static uint64_t inv_rshift_xor(uint64_t value, uint32_t shift);

  static uint64_t inv_fmix(uint64_t k);

  static uint64_t reverse_bytes(uint64_t x);
};

} // namespace dibibase::dht
