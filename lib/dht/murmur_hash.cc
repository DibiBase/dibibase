#include <cstdint>
#include <memory>

#include "dht/murmur_hash.hh"

namespace dibibase::dht {

uint32_t MurmurHash::hash2_x32(uint8_t key[], uint32_t offset, uint32_t length,
                              uint32_t seed) {
  uint32_t m = 0x5bd1e995;
  uint32_t r = 24;

  uint32_t h = seed ^ length;

  uint32_t len_4 = length >> 2;

  for (uint32_t i = 0; i < len_4; i++) {
    uint32_t i_4 = i << 2;
    uint32_t k = key[offset + i_4 + 3];
    k = k << 8;
    k = k | (key[offset + i_4 + 2] & 0xff);
    k = k << 8;
    k = k | (key[offset + i_4 + 1] & 0xff);
    k = k << 8;
    k = k | (key[offset + i_4 + 0] & 0xff);
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
  }

  // avoid calculating modulo
  uint32_t len_m = len_4 << 2;
  uint32_t left = length - len_m;

  if (left != 0) {
    if (left >= 3) {
      h ^= (uint32_t)key[offset + length - 3] << 16;
    }
    if (left >= 2) {
      h ^= (uint32_t)key[offset + length - 2] << 8;
    }
    if (left >= 1) {
      h ^= (uint32_t)key[offset + length - 1];
    }

    h *= m;
  }

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

uint64_t MurmurHash::hash2_x64(uint8_t key[], uint32_t offset, uint32_t length,
                              uint64_t seed) {
  uint64_t m64 = 0xc6a4a7935bd1e995L;
  uint32_t r64 = 47;

  uint64_t h64 = (seed & 0xffffffffL) ^ (m64 * length);

  uint32_t lenLongs = length >> 3;

  for (uint32_t i = 0; i < lenLongs; ++i) {
    uint32_t i_8 = i << 3;

    uint64_t k64 = ((uint64_t)key[offset + i_8 + 0] & 0xff) +
                   (((uint64_t)key[offset + i_8 + 1] & 0xff) << 8) +
                   (((uint64_t)key[offset + i_8 + 2] & 0xff) << 16) +
                   (((uint64_t)key[offset + i_8 + 3] & 0xff) << 24) +
                   (((uint64_t)key[offset + i_8 + 4] & 0xff) << 32) +
                   (((uint64_t)key[offset + i_8 + 5] & 0xff) << 40) +
                   (((uint64_t)key[offset + i_8 + 6] & 0xff) << 48) +
                   (((uint64_t)key[offset + i_8 + 7] & 0xff) << 56);

    k64 *= m64;
    k64 ^= k64 >> r64;
    k64 *= m64;

    h64 ^= k64;
    h64 *= m64;
  }

  uint32_t rem = length & 0x7;

  switch (rem) {
  case 0:
    break;
  case 7:
    h64 ^= (uint64_t)key[offset + length - rem + 6] << 48;
  case 6:
    h64 ^= (uint64_t)key[offset + length - rem + 5] << 40;
  case 5:
    h64 ^= (uint64_t)key[offset + length - rem + 4] << 32;
  case 4:
    h64 ^= (uint64_t)key[offset + length - rem + 3] << 24;
  case 3:
    h64 ^= (uint64_t)key[offset + length - rem + 2] << 16;
  case 2:
    h64 ^= (uint64_t)key[offset + length - rem + 1] << 8;
  case 1:
    h64 ^= (uint64_t)key[offset + length - rem];
    h64 *= m64;
  }

  h64 ^= h64 >> r64;
  h64 *= m64;
  h64 ^= h64 >> r64;

  return h64;
}

std::unique_ptr<uint64_t[]> MurmurHash::hash3_x64_128(uint8_t key[],
                                                      uint32_t offset,
                                                      uint32_t length,
                                                      uint64_t seed) {
  uint32_t nblocks = length >> 4; // Process as 128-bit blocks.

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  uint64_t c1 = 0x87c37b91114253d5L;
  uint64_t c2 = 0x4cf5ad432745937fL;

  //----------
  // body

  for (uint32_t i = 0; i < nblocks; i++) {
    uint64_t k1 = get_block(key, offset, i * 2 + 0);
    uint64_t k2 = get_block(key, offset, i * 2 + 1);

    k1 *= c1;
    k1 = rotl64(k1, 31);
    k1 *= c2;
    h1 ^= k1;

    h1 = rotl64(h1, 27);
    h1 += h2;
    h1 = h1 * 5 + 0x52dce729;

    k2 *= c2;
    k2 = rotl64(k2, 33);
    k2 *= c1;
    h2 ^= k2;

    h2 = rotl64(h2, 31);
    h2 += h1;
    h2 = h2 * 5 + 0x38495ab5;
  }

  //----------
  // tail

  // Advance offset to the unprocessed tail of the data.
  offset += nblocks * 16;

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch (length & 15) {
  case 15:
    k2 ^= ((uint64_t)key[offset + 1]) << 48;
  case 14:
    k2 ^= ((uint64_t)key[offset + 1]) << 40;
  case 13:
    k2 ^= ((uint64_t)key[offset + 1]) << 32;
  case 12:
    k2 ^= ((uint64_t)key[offset + 1]) << 24;
  case 11:
    k2 ^= ((uint64_t)key[offset + 1]) << 16;
  case 10:
    k2 ^= ((uint64_t)key[offset + 9]) << 8;
  case 9:
    k2 ^= ((uint64_t)key[offset + 8]) << 0;
    k2 *= c2;
    k2 = rotl64(k2, 33);
    k2 *= c1;
    h2 ^= k2;

  case 8:
    k1 ^= ((uint64_t)key[offset + 7]) << 56;
  case 7:
    k1 ^= ((uint64_t)key[offset + 6]) << 48;
  case 6:
    k1 ^= ((uint64_t)key[offset + 5]) << 40;
  case 5:
    k1 ^= ((uint64_t)key[offset + 4]) << 32;
  case 4:
    k1 ^= ((uint64_t)key[offset + 3]) << 24;
  case 3:
    k1 ^= ((uint64_t)key[offset + 2]) << 16;
  case 2:
    k1 ^= ((uint64_t)key[offset + 1]) << 8;
  case 1:
    k1 ^= ((uint64_t)key[offset]);
    k1 *= c1;
    k1 = rotl64(k1, 31);
    k1 *= c2;
    h1 ^= k1;
  };

  //----------
  // zation

  h1 ^= length;
  h2 ^= length;

  h1 += h2;
  h2 += h1;

  h1 = fmix(h1);
  h2 = fmix(h2);

  h1 += h2;
  h2 += h1;

  return std::unique_ptr<uint64_t[]>(new uint64_t[2]{h1, h2});
}

std::unique_ptr<uint64_t[]> MurmurHash::inv_hash3_x64_128(uint64_t result[]) {
  uint64_t c1 = 0xa98409e882ce4d7dL;
  uint64_t c2 = 0xa81e14edd9de2c7fL;

  uint64_t k1 = 0;
  uint64_t k2 = 0;
  uint64_t h1 = result[0];
  uint64_t h2 = result[1];

  //----------
  // reverse zation
  h2 -= h1;
  h1 -= h2;

  h1 = inv_fmix(h1);
  h2 = inv_fmix(h2);

  h2 -= h1;
  h1 -= h2;

  h1 ^= 16;
  h2 ^= 16;

  //----------
  // reverse body
  h2 -= 0x38495ab5;
  h2 *= 0xcccccccccccccccdL;
  h2 -= h1;
  h2 = inv_rotl64(h2, 31);
  k2 = h2;
  h2 = 0;

  k2 *= c1;
  k2 = inv_rotl64(k2, 33);
  k2 *= c2;

  h1 -= 0x52dce729;
  h1 *= 0xcccccccccccccccdL;
  // h1 -= h2;
  h1 = inv_rotl64(h1, 27);

  k1 = h1;

  k1 *= c2;
  k1 = inv_rotl64(k1, 31);
  k1 *= c1;

  // note that while this works for body block reversing the tail reverse
  // requires `invTailReverse`
  k1 = reverse_bytes(k1);
  k2 = reverse_bytes(k2);

  return std::unique_ptr<uint64_t[]>(new uint64_t[2]{k1, k2});
}

uint64_t MurmurHash::get_block(uint8_t key[], uint32_t offset, uint32_t index) {
  uint32_t i_8 = index << 3;
  uint32_t blockOffset = offset + i_8;
  return ((uint64_t)key[blockOffset + 0] & 0xff) +
         (((uint64_t)key[blockOffset + 1] & 0xff) << 8) +
         (((uint64_t)key[blockOffset + 2] & 0xff) << 16) +
         (((uint64_t)key[blockOffset + 3] & 0xff) << 24) +
         (((uint64_t)key[blockOffset + 4] & 0xff) << 32) +
         (((uint64_t)key[blockOffset + 5] & 0xff) << 40) +
         (((uint64_t)key[blockOffset + 6] & 0xff) << 48) +
         (((uint64_t)key[blockOffset + 7] & 0xff) << 56);
}

uint64_t MurmurHash::rotl64(uint64_t v, uint32_t n) {
  return ((v << n) | (v >> (64 - n)));
}

uint64_t MurmurHash::fmix(uint64_t k) {
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccdL;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53L;
  k ^= k >> 33;

  return k;
}

uint64_t MurmurHash::inv_rotl64(uint64_t v, uint32_t n) {
  return ((v >> n) | (v << (64 - n)));
}

uint64_t MurmurHash::inv_rshift_xor(uint64_t value, uint32_t shift) {
  uint64_t output = 0;
  uint64_t i = 0;
  while (i * shift < 64) {
    uint64_t c = (uint64_t)(0xffffffffffffffffL << (64 - shift)) >> (shift * i);
    uint64_t partOutput = value & c;
    value ^= partOutput >> shift;
    output |= partOutput;
    i += 1;
  }
  return output;
}

uint64_t MurmurHash::inv_fmix(uint64_t k) {
  k = inv_rshift_xor(k, 33);
  k *= 0x9cb4b2f8129337dbL;
  k = inv_rshift_xor(k, 33);
  k *= 0x4f74430c22a54005L;
  k = inv_rshift_xor(k, 33);
  return k;
}

uint64_t MurmurHash::reverse_bytes(uint64_t x) {
  x = (x & 0x00000000FFFFFFFF) << 32 | (x & 0xFFFFFFFF00000000) >> 32;
  x = (x & 0x0000FFFF0000FFFF) << 16 | (x & 0xFFFF0000FFFF0000) >> 16;
  x = (x & 0x00FF00FF00FF00FF) << 8 | (x & 0xFF00FF00FF00FF00) >> 8;
  return x;
}

} // namespace dibibase::dht
