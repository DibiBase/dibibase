#include "gtest/gtest.h"

#include "catalog/schema.hh"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
using namespace dibibase::util;
using namespace dibibase::catalog;

////////////////// Field Tests //////////////////////
TEST(Field, allfieldfns) {
  // valid stub creation
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(2);
  std::unique_ptr<unsigned char[]> myUniquePtr =
      std::unique_ptr<unsigned char[]>(new unsigned char('a'));
  std::unique_ptr<unsigned char[]> ptr =
      std::unique_ptr<unsigned char[]>(new unsigned char('a'));
  BlobData blobdata(std::move(myUniquePtr), 2);
  blobdata.bytes(buf.get());
  std::unique_ptr<unsigned char[]> bytes = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(bytes), 2);
  std::unique_ptr<Data> d =
      Data::from(buf1.get(), Data::Type(Data::Type::BLOB, 2));
  // actual Field testing
  Field f("field name", d->type());
  std::string f_name = f.name();
  Data::Type f_type = f.type();
  EXPECT_EQ(f_name, "field name");
  EXPECT_EQ(f_type.id(), Data::Type::BLOB);

  Buffer *field_buf = new MemoryBuffer(f.size());
  f.bytes(field_buf);
  std::unique_ptr<unsigned char[]> buffer_ptr = field_buf->bytes();
  std::unique_ptr<Buffer> buf22 =
      std::make_unique<MemoryBuffer>(move(buffer_ptr), f.size());
  std::unique_ptr<Field> field_ptr = Field::from(buf22.get());
  EXPECT_EQ(field_ptr->type().id(), Data::Type::BLOB);
}

////////////////// Schema Tests //////////////////////
TEST(Schema, schematest) {
  // valid stub creation
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 =
      std::make_unique<MemoryBuffer>(move(test), 255);
  std::unique_ptr<Data> d =
      Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 255));
  // actual Field testing
  std::vector<Field> fieldvector;
  std::string fieldname = "";
  int total_size = 0;
  for (int i = 0; i < 10; i++) {
    fieldname = "Field No" + std::to_string(i);
    Field f(fieldname, d->type());
    total_size += f.size();
    fieldvector.push_back(f);
  }
  total_size += sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint8_t);
  Schema s(2, 3, fieldvector);
  EXPECT_EQ(s.sort_key_index(), 2);
  EXPECT_EQ(s.partition_key_index(), 3);
  EXPECT_EQ(s.size(), total_size);
}
