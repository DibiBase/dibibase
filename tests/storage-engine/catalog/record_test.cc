#include "gtest/gtest.h"

#include "catalog/data.hh"
#include "catalog/record.hh"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
using namespace dibibase::util;
using namespace dibibase::catalog;

TEST(Record, allrecordfns) {
  // initializing record constructor //
  std::vector<std::shared_ptr<Data>> datav;
  std::vector<Field> fieldvector;
  std::shared_ptr<Data> data1 = std::make_shared<ASCIIData>("data1");
  std::shared_ptr<Data> data2 =
      std::make_shared<BigIntData>(-9223372036854775807);
  std::shared_ptr<Data> data3 = std::make_shared<IntData>(9999);
  std::shared_ptr<Data> data4 = std::make_shared<SmallIntData>(5);
  std::shared_ptr<Data> data5 =
      std::make_shared<DoubleData>(1.7976931348623157E+308);
  datav.push_back(data1);
  datav.push_back(data2);
  datav.push_back(data3);
  datav.push_back(data4);
  datav.push_back(data5);
  Record r(datav);
  Schema s(0,0);
  Field f1("f0", Data::Type(Data::Type::ASCII, 5));
  Field f2("f1", Data::Type(Data::Type::BIGINT, sizeof(int64_t)));
  Field f3("f2", Data::Type(Data::Type::INT, sizeof(int32_t)));
  Field f4("f3", Data::Type(Data::Type::SMALLINT, sizeof(int16_t)));
  Field f5("f4", Data::Type(Data::Type::DOUBLE, sizeof(double)));
  s.push_back(f1);
  s.push_back(f2);
  s.push_back(f3);
  s.push_back(f4);
  s.push_back(f5);

  EXPECT_EQ(r.values(), datav);

  // testing bytes & from fns //
  Buffer *table_buf = new MemoryBuffer(datav.size() + 22);
  r.bytes(table_buf);
  std::unique_ptr<unsigned char[]> buffer_ptr = table_buf->bytes();
  std::unique_ptr<Buffer> buf22 =
      std::make_unique<MemoryBuffer>(move(buffer_ptr), datav.size() + 22);
  std::unique_ptr<Record> record_ptr = Record::from(buf22.get(), s);
    std::vector<std::shared_ptr<Data>> output_values =record_ptr->values();
  for (int i=0 ; i<5;i++){
  std::shared_ptr<Data> ex_data_shared_ptr = datav[i];
  std::shared_ptr<Data> out_data_shared_ptr = record_ptr->values()[i]; 
  EXPECT_EQ(out_data_shared_ptr->type().id(),ex_data_shared_ptr->type().id());
  EXPECT_EQ(out_data_shared_ptr->type().size(),ex_data_shared_ptr->type().size());  
  }

    //NB: Expected and output shared Data ptrs now point to different locations in memory
    /////they only have same values ///// thus this test "EXPECT_EQ(out_data_shared_ptr.get(), ex_data_shared_ptr.get())" will fail 
}