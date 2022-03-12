#include "gtest/gtest.h"

#include "catalog/data.hh"
#include "db/database.hh"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <memory>
#include <iostream>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <bits/stdc++.h>

using namespace dibibase::util;
using namespace dibibase::db;
using namespace dibibase::catalog;

TEST(Database , dbconsttabletest){
    // creating a stub //
  Schema s(0,0);
  Field f1("f0", Data::Type(Data::Type::ASCII, 5));
  s.push_back(f1);

  Database db("/test");
  db.create_table("table1",s);

  
    // random record insertion //
  std::vector<std::shared_ptr<Data>> datav;
  std::shared_ptr<Data> data1 = std::make_shared<ASCIIData>("data1");
  datav.push_back(data1);
  Record r(datav);
  for (int i=0 ; i<10;i++)
  db.write_record("table1", r);


    //reading written Data //
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(255);
  std::string expected = "";
  for (int i=0 ; i<255 ; i++)
    expected += "a";
   
  ASCIIData ascii_data(expected);
  ascii_data.bytes(buf.get());
  std::unique_ptr<unsigned char[]> test = buf->bytes();
  std::unique_ptr<Buffer> buf1 = std::make_unique<MemoryBuffer>(move(test), 255);
  std::unique_ptr<Data> d_ptr = Data::from(buf1.get(), Data::Type(Data::Type::ASCII, 255));

  //db.read_record("table1", std::move(d_ptr));
    

}

TEST (Database , flushed){



}