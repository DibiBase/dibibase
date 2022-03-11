#include "gtest/gtest.h"

#include "catalog/schema.hh"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
using namespace dibibase::util;
using namespace dibibase::catalog;

////////////////// Field Tests //////////////////////
TEST(Field, constructor) {
  // valid stub creation
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
  std::string expected = "aaaaaaaaaaaaaaaa";
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 =
      std::make_unique<MemoryBuffer>(move(test), 255);
  std::unique_ptr<Data> d =
      Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 255));
  // actual Field testing
  Field f("field name", d->type());
  std::string f_name = f.name();
  Data::Type f_type = f.type();
  EXPECT_EQ(f_name, "field name");
  EXPECT_EQ(f_type.id(), Data::Type::ASCII);
}
TEST(Field, fieldfromtest) {
  // from function has some errors
  std::unique_ptr<Buffer> buf2 = std::make_unique<MemoryBuffer>(255);
  std::unique_ptr<unsigned char[]> myUniquePtr =
      std::unique_ptr<unsigned char[]>(new unsigned char('a'));
  BlobData blobdata(std::move(myUniquePtr), 255);
  blobdata.bytes(buf2.get());
  std::unique_ptr<unsigned char[]> bytes = buf2->bytes();
  std::unique_ptr<Buffer> buf21 =
      std::make_unique<MemoryBuffer>(move(bytes), 255);
  std::unique_ptr<Field> f_ptr = Field::from(buf21.get());

  // Always returns ASCII data type
  EXPECT_EQ(f_ptr->type().id(), Data::Type::BLOB);
}

////////////////// Schema Tests //////////////////////
