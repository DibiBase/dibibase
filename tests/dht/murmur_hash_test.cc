#include "gtest/gtest.h"
#include <cstdint>

#include "dht/murmur_hash.hh"

using namespace dibibase::dht;

TEST(MurmurHash, hash2_32) {
  uint8_t key[] = "hello";
  uint32_t result = MurmurHash::hash2_x32(key, 0, 5, 0x982);
  uint32_t expected_result = 0x1E65961D;
  EXPECT_EQ(result, expected_result);
}

TEST(MurmurHash, hash2_64) {
  uint8_t key[] = "We are here";
  uint64_t result = MurmurHash::hash2_x64(key, 0, 11, 0x14F);
  uint64_t expected_result = 0x7A647FDF36B1E653;
  EXPECT_EQ(result, expected_result);
}

TEST(MurmurHash, hash3_x64_128) {
  uint8_t key[] = "Yes We are here";
  std::unique_ptr<uint64_t[]> result =
      MurmurHash::hash3_x64_128(key, 0, 15, 0xFAE);
  uint64_t expected_result[] = {0x65A621C0D6CC7865, 0x7EDC0F9358782055};
  EXPECT_EQ(result[0], expected_result[0]);
  EXPECT_EQ(result[1], expected_result[1]);
}

TEST(MurmurHash, inv_hash3_x64_128) {
  uint64_t res[] = {0x0, 0x0};
  std::unique_ptr<uint64_t[]> result = MurmurHash::inv_hash3_x64_128(res);
  uint64_t expected_result[] = {0x5ED0B893F10C088C, 0xCF0E007BF6EDBF61};
  EXPECT_EQ(result[0], expected_result[0]);
  EXPECT_EQ(result[1], expected_result[1]);
}
