#pragma once

#include <cstdint>

#include "api/cql3/server.hh"
#include "common.hh"
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
namespace dibibase::api::cql3 {
class DIBIBASE_PUBLIC Metadata {
public:
  Metadata(unsigned char *buff) { body = buff; }
  int meta_result (std::string);
  void append(unsigned char *,unsigned char * ,int);
  int add_schema(unsigned char *,std::string* ss, int* , int* , int*,int cols,int count);
  int append_hdrs(unsigned char *,std::string , std::string,int cols);
  int add_str(unsigned char *,std::string, int count);
  int record_metadata(unsigned char * , int count , int rows);
  int add_records(unsigned char* , int count , std::string* , int rows);
  int record_size(unsigned char* , int count, std::string);
  int total_size(unsigned char *,int count);
  
private:
  unsigned char *body = {nullptr};
  std::string keyspace;
  std::string table;
  std::string peers = "peers_v2";
  std::string local = "system.local";
  std::string system_keyspaces = "system_schema.keyspaces";
  std::string aggregates = "aggregates";
  std::string functions = "functions";
  std::string triggers = "triggers";
  std::string tables = "system_schema.tables";
  std::string columns = "system_schema.columns";
  std::string views = "views";
  std::string types = "types";
  std::string indexes = "indexes";
  std::string v_keyspaces = "system_virtual_schema.keyspaces";
  std::string v_columns = "system_virtual_schema.columns";
  std::string v_tables = "system_virtual_schema.tables";
};

} // namespace dibibase::api::cql3