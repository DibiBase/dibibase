#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include "util/buffer.hh"
using namespace dibibase::util;

TEST(MemoryBuffer , membuffuint8){
    MemoryBuffer m(15);
    m.put_uint8(4);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 15);
    uint8_t out = membuff.get_uint8();
    EXPECT_EQ(out, 4);
}
TEST(MemoryBuffer , membuffuint16){
    MemoryBuffer m(16);
    m.put_uint16(65535);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 16);
    uint16_t expected = 65535;
    uint16_t out = membuff.get_uint16();
    EXPECT_EQ(out, expected);
}
TEST(MemoryBuffer , membuffuint32){
    MemoryBuffer m(16);
    m.put_uint32(4294967295);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 16);
    uint32_t expected = 4294967295;
    uint32_t out = membuff.get_uint32();
    EXPECT_EQ(out, expected);
}
TEST(MemoryBuffer , membuffuint64){
    MemoryBuffer m(16);
    m.put_uint64(8446744073709551615);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 16);
    uint64_t expected = 8446744073709551615;
    uint64_t out = membuff.get_uint64();
    EXPECT_EQ(out, expected);
}

TEST(MemoryBuffer , membuffint64){
    MemoryBuffer m(16);
    m.put_int64(-223372036854775808);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 16);
    int64_t expected = -223372036854775808;
    int64_t out = membuff.get_int64();
    EXPECT_EQ(out, expected);
}
TEST(MemoryBuffer , membuffblob){
    MemoryBuffer m(16);
    unsigned char some_char = 'a';
    m.put_blob(&some_char,5);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 16);
    unsigned char expected = 'a';
    std::unique_ptr<unsigned char[]> out= membuff.get_blob(5);
    EXPECT_EQ(*out.get(), expected);
}

TEST(MemoryBuffer , membuffsize){
    MemoryBuffer m(255);
    std::unique_ptr<unsigned char[]> buffer = m.bytes();
    MemoryBuffer membuff(std::move(buffer), 255);
    size_t expected = 255;
    size_t out= membuff.size();
    EXPECT_EQ(out, expected);
}