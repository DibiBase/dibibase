#pragma once

#include <cstdint>

#include "common.hh"

namespace dibibase::api::cql3 {

class DIBIBASE_PUBLIC Frame {

public:
  Frame(char *, int);
  void parse();
  char *substr(char *, int, int);
  char version, flags, opcode;
  unsigned char streamID[2] = {
      '0', '0'}; // initially filled with zeroes instead of null

  int max_size;
  char *body{nullptr};

private:
  char *buffer{nullptr};
  char length[4] = {'0', '0', '0', '0'}; // also initially filled with zeroes
};

} // namespace dibibase::api::cql3
