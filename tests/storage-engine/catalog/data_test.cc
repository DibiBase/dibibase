#include "gtest/gtest.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include "catalog/data.hh"
using namespace dibibase::util;
using namespace dibibase::catalog;

TEST(Type , asciitype){
std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
  std::string expected = "";
  for (int i=0 ; i<255 ; i++)
    expected += "a";
   
  ASCIIData ascii_data(expected);
  ascii_data.bytes(buf.get());
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(test), 255);
  std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 255));
  Data::Type t = d->type();
    // if dynamic_cast failed, a memory leak occurs
    std::unique_ptr<ASCIIData> child_data(
        dynamic_cast<ASCIIData *>(d.release()));
    std::string out = child_data->data();  
    EXPECT_EQ(t.id(), Data::Type::ASCII); 
    EXPECT_EQ(out, expected);
    //always return 5 i guess
    EXPECT_EQ(t.size(),expected.size());
}
TEST(Type , biginttype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    std::int64_t expected = -9223372036854775807;
    BigIntData bigintdata(expected);
    bigintdata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::BIGINT, 255));
    Data::Type t = d->type();
    std::unique_ptr<BigIntData> child_data(
        dynamic_cast<BigIntData *>(d.release()));
    std::uint64_t out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::BIGINT); 
    EXPECT_EQ(out, expected);
}
TEST(Type , booltype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(2);
    bool expected = false;
    BooleanData booldata(expected);
    booldata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 2);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::BOOLEAN, 2));
    Data::Type t = d->type();
    std::unique_ptr<BooleanData> child_data(
        dynamic_cast<BooleanData *>(d.release()));
    bool out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::BOOLEAN); 
    EXPECT_EQ(out, expected);
}
TEST(Type , doubletype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    double expected = 1.7976931348623157E+308;
    DoubleData doubledata(expected);
    doubledata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::DOUBLE, 255));
    Data::Type t = d->type();
    std::unique_ptr<DoubleData> child_data(
        dynamic_cast<DoubleData *>(d.release()));
    double out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::DOUBLE); 
    EXPECT_EQ(out, expected);
}
TEST(Type , floattype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    float expected = 100000.7976931348623157;
    FloatData floatdata(expected);
    floatdata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::FLOAT, 255));
    Data::Type t = d->type();
    std::unique_ptr<FloatData> child_data(
        dynamic_cast<FloatData *>(d.release()));
    float out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::FLOAT); 
    EXPECT_EQ(out, expected);
}
TEST(Type , inttype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    int expected = 0x7FFFFFFF;
    IntData intdata(expected);
    intdata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::INT, 255));
    Data::Type t = d->type();
    std::unique_ptr<IntData> child_data(
        dynamic_cast<IntData *>(d.release()));
    float out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::INT); 
    EXPECT_EQ(out, expected);
}
TEST(Type , smallinttype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    int16_t expected = 32767;
    SmallIntData smallint(expected);
    smallint.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::SMALLINT, 255));
    Data::Type t = d->type();
    std::unique_ptr<SmallIntData> child_data(
        dynamic_cast<SmallIntData *>(d.release()));
    float out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::SMALLINT); 
    EXPECT_EQ(out, expected);
}
TEST(Type , tinyinttype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
    int8_t expected = -126;
    TinyIntData smallint(expected);
    smallint.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 255);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::TINYINT, 255));
    Data::Type t = d->type();
    std::unique_ptr<TinyIntData> child_data(
        dynamic_cast<TinyIntData *>(d.release()));
    float out = child_data->data();
    EXPECT_EQ(t.id(), Data::Type::TINYINT); 
    EXPECT_EQ(out, expected);
}
TEST(Type , blobtype){
    std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(2);
    unsigned char expected = 'a';
    std::unique_ptr<unsigned char[]> myUniquePtr = std::unique_ptr<unsigned char[]>(new unsigned char('a'));
    std::unique_ptr<unsigned char[]> ptr =std::unique_ptr<unsigned char[]>(new unsigned char('a')); 
    BlobData blobdata(std::move(myUniquePtr),2);
    blobdata.bytes(buf.get());
    std::unique_ptr<unsigned char[]> bytes = buf->bytes();
    std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 2);
    std::unique_ptr<Data> d = Data::from(buf1.get(), Data::Type(Data::Type::BLOB, 2));
    Data::Type t = d->type();
    std::unique_ptr<BlobData> child_data(
        dynamic_cast<BlobData *>(d.release()));
    std::unique_ptr<unsigned char[]>  out = child_data->data();
    unsigned char *output_ptr = out.release();

    EXPECT_EQ(t.id(), Data::Type::BLOB); 
    EXPECT_EQ(*output_ptr, expected);
}
