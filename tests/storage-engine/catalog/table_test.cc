#include "gtest/gtest.h"

#include "catalog/table.hh"
#include "util/buffer.hh"
#include <cstddef>
#include <cstdint>
#include <limits>
using namespace dibibase::util;
using namespace dibibase::catalog;

TEST(Table, tabletest) {

  // valid stub creation
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 =
      std::make_unique<MemoryBuffer>(move(test), 255);
  std::unique_ptr<Data> d =
      Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 255));
  std::vector<Field> fieldvector;
  std::string fieldname = "";
  for (int i = 0; i < 10; i++) {
    fieldname = "Field No" + std::to_string(i);
    Field f(fieldname, d->type());
    fieldvector.push_back(f);
  }
  Schema s(2, 3, fieldvector);

  // constructor Testing //
  std::string tablename = "table name";
  Table t(tablename, s, 10);

  EXPECT_EQ(t.name(), tablename);
  EXPECT_EQ(t.last_sstable_id(), 10);
  EXPECT_EQ(t.size(),
            tablename.length() + sizeof(uint8_t) + sizeof(uint64_t) + s.size());
  // from & bytes test //
  Buffer *table_buf = new MemoryBuffer(t.size());
  t.bytes(table_buf);
  std::unique_ptr<unsigned char[]> buffer_ptr = table_buf->bytes();
  std::unique_ptr<Buffer> buf22 =
      std::make_unique<MemoryBuffer>(move(buffer_ptr), t.size());
  std::unique_ptr<Table> table_ptr = Table::from(buf22.get());
  EXPECT_EQ(table_ptr->name(), tablename);
  EXPECT_EQ(table_ptr->last_sstable_id() , 10);
}