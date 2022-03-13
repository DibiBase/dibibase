#include "gtest/gtest.h"

#include "catalog/data.hh"
#include "db/database.hh"
#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <unistd.h>

using namespace dibibase::util;
using namespace dibibase::db;
using namespace dibibase::catalog;

TEST(Database, dbasciitest) {
  // creating a stub //

 Schema s(0, 0);
  Field f1("f1", Data::Type(Data::Type::ASCII,5));
  s.push_back(f1);
  char tmpdir[256];
  getcwd(tmpdir, 256);
  Database db(std::string(tmpdir) + "/test1");
  db.create_table("table1", s);

  // random record insertion //
  std::vector<std::shared_ptr<Data>> datav;
  std::vector<std::shared_ptr<Data>> datal;
  std::vector<std::shared_ptr<Data>> datar;
  std::shared_ptr<Data> data1 = std::make_shared<ASCIIData>("data1");
  std::shared_ptr<Data> data2 = std::make_shared<ASCIIData>("data2");
  std::shared_ptr<Data> data3 = std::make_shared<ASCIIData>("data3");
  datav.push_back(data1);
  datal.push_back(data2);
  datar.push_back(data3);
  Record r(datav);
  Record r2(datal);
  Record r3(datar);
  db.write_record("table1", r);
  db.write_record("table1", r2);
  db.write_record("table1", r3);

  // reading written Data //

  Buffer *table_buf = new MemoryBuffer(datav.size() + 22);
  r.bytes(table_buf);

  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(5);

  ASCIIData ascii_data("data1");
  ascii_data.bytes(buf.get());
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(test), 5);
  std::unique_ptr<Data> d_ptr = Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 5));
  std::vector<std::shared_ptr<Data>> data_out;
  data_out = db.read_record("table1", std::move(d_ptr)).values();
  EXPECT_EQ(data_out.front()->type().id(), datav.front()->type().id());
  EXPECT_EQ(data_out.front()->type().length(), datav.front()->type().length());
}

TEST(Database, dbintrectest) {
  // creating a stub //
   std::cout  << sizeof(int32_t);
  std::int32_t somint = 5000;
  std::int32_t another = 3000;
  std::int32_t third = 7000;

 Schema s(0, 0);
  Field f1("column2", Data::Type(Data::Type::INT, sizeof(int32_t)));
  s.push_back(f1);
  char tmpdir[256];
  getcwd(tmpdir, 256);
  Database db(std::string(tmpdir) + "/test2");
  db.create_table("table2", s);

  // random record insertion //
  std::vector<std::shared_ptr<Data>> datav;
  std::vector<std::shared_ptr<Data>> datal;
  std::vector<std::shared_ptr<Data>> datar;
  std::shared_ptr<Data> data1 = std::make_shared<IntData>(somint);
  std::shared_ptr<Data> data2 = std::make_shared<IntData>(another);
  std::shared_ptr<Data> data3 = std::make_shared<IntData>(third);
  datav.push_back(data1);
  datal.push_back(data2);
  datar.push_back(data3);
  Record r(datav);
  Record r2(datal);
  Record r3(datar);
  Record r4(datav);
  db.write_record("table2", r);
  db.write_record("table2", r2);
  db.write_record("table2", r3);
  // reading written Data //
  Buffer *table_buf = new MemoryBuffer(datav.size() + 22);
 r.bytes(table_buf);
 std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(datav.size() + 22);
  IntData intdata(somint);
  intdata.bytes(buf.get());
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(test), sizeof(int32_t));
  std::unique_ptr<Data> d_ptr = Data::from(buf1.get(), Data::Type(Data::Type::INT, sizeof(int32_t)));
  std::vector<std::shared_ptr<Data>> data_out;
  data_out = db.read_record("table2", std::move(d_ptr)).values(); /////////////no read record /////
  EXPECT_EQ(r.values().size(), data_out.size()); //de tmam
  EXPECT_EQ(r.values()[0]->type().id(), data_out[0]->type().id());
  EXPECT_EQ(data_out.front()->type().length(), datav.front()->type().length());
}
TEST(Database, dbdoubletest) {
  // creating a stub //
  Schema s(0, 0);
  Field f1("f1", Data::Type(Data::Type::DOUBLE, sizeof(double)));
  s.push_back(f1);
  char tmpdir[256];
  getcwd(tmpdir, 256);
  Database db(std::string(tmpdir) + "/test3");
  db.create_table("table3", s);

  // random record insertion //
  std::vector<std::shared_ptr<Data>> datav;
  std::vector<std::shared_ptr<Data>> datal;
  std::vector<std::shared_ptr<Data>> datar;
  double d1 = 10.11564,d2=5.45,d3=241;
  std::shared_ptr<Data> data1 = std::make_shared<DoubleData>(d1);
  std::shared_ptr<Data> data2 = std::make_shared<DoubleData>(d2);
  std::shared_ptr<Data> data3 = std::make_shared<DoubleData>(d3);
  datav.push_back(data1);
  datal.push_back(data2);
  datar.push_back(data3);
  Record r(datav);
  Record r2(datal);
  Record r3(datar);
  db.write_record("table3", r);
  db.write_record("table3", r2);
  db.write_record("table3",r3);
  // reading written Data //
  Buffer *table_buf = new MemoryBuffer(datav.size() + 22);
  r.bytes(table_buf);
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(sizeof(double));
  DoubleData doubledata(d2);
  doubledata.bytes(buf.get());
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(test), 30);
  std::unique_ptr<Data> d_ptr =
      Data::from(buf1.get(), Data::Type(Data::Type::DOUBLE, 30));
  std::vector<std::shared_ptr<Data>> data_out;
  std::cout << d_ptr.get()->type().id();

  data_out = db.read_record("table3", std::move(d_ptr)).values();
  EXPECT_EQ(data_out.front()->type().id(), datav.front()->type().id());
  EXPECT_EQ(data_out.front()->type().length(), datav.front()->type().length());
}