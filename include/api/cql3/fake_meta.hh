#pragma once

#include <cstdint>

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
private:
  unsigned char *body = {nullptr};
  std::string pquery;
  std::string peers = "peers";
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
  std::string system_virual_schema = "system_virtual_schema.columns";
  std::string v_keyspaces = "system_virtual_schema.keyspaces";
  std::string v_columns = "system_virtual_schema.columns";
  std::string v_tables = "system_virtual_schema.tables";
};

} // namespace dibibase::api::cql3