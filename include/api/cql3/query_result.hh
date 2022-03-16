#pragma once

#include "common.hh"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace dibibase::api::cql3 {

class DIBIBASE_PUBLIC QueryResult {
public:
  QueryResult(std::string keyspace,std::string table,unsigned char *);
  /*
    Putting initial Kind , flag  , and metadata_flags of the given keyspace.table
  */
  void put_ascii(std::string,unsigned char *);
  void put_boolean(bool,unsigned char *);
  void put_bigint(int64_t,unsigned char *);
  void put_int (int, unsigned char *);
  void put_double(double,unsigned char *);
  void put_float(float,unsigned char *);
  void put_varchar(std::string,unsigned char *);
  void put_smallint(int16_t,unsigned char *);
  void put_tinyint(int8_t,unsigned char *);
  void put_map(std::map<std::string,std::string>,unsigned char *);  //Not implemented yet (might need it for meta tables) //

private:
  enum kind {
    Void = 0x0001,
    Rows = 0x0002,
    Set_Keyspace = 0x0003,
    Prepared = 0x0004,
    Schema_change = 0x0005
  };

  enum flag { NONE = 0x00, COMPRESSED = 0x01, TRACING = 0x02 };
  enum metadata_flags {
    Global_tables_spec = 0x0001,
    Has_more_pages = 0x0002,    /**<not the last page of results. */
    No_metadata = 0x0004        
  };
  /*<Schema Count> <Schema Binary Data> <Schema type> */
    /*  Count For Schema takes 2 decimal places*/
  /*<Record Count> <Record Binary Data> */
    /* Count For Record takes 4 decimal places*/
  enum type {
    Custom = 0x0000,
    Ascii = 0x0001,
    Bigint = 0x0002,
    Blob = 0x0003,
    Boolean = 0x0004,
    Counter = 0x0005,
    Decimal = 0x0006,
    Double = 0x0007,
    Float = 0x0008,
    Int = 0x0009,
    Timestamp = 0x000B,
    Uuid = 0x000C,
    Varchar = 0x000D,
    Varint = 0x000E,
    Timeuuid = 0x000F,
    Inet = 0x0010,
    Date = 0x0011,
    Time = 0x0012,
    Smallint = 0x0013,
    Tinyint = 0x0014,
    List = 0x0020,
    Map = 0x0021,
    Set = 0x0022,
    UDT = 0x0030,
    Tuple = 0x0031
  };

};

} // namespace dibibase::api::cql3
