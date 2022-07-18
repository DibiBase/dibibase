#include "api/cql3/frame.hh"
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
using namespace dibibase::api::cql3;

using namespace std;

Frame::Frame(char *buff, int size) {
  buffer = buff;
  version = 0x03; // hardcoding the version (not the best way but there is quite
                  // difference between versions)
  max_size = size;
}
/*
 *parse() is used to divide the header bytes of the incoming buffer to get the
 *needed bytes and assign them to the prevously declared vars.
 */
void Frame::parse() {
  version = *buffer;
  flags = buffer[1];

  streamID[0] = buffer[2];
  streamID[1] = buffer[3];

  opcode = buffer[4];

  length[0] = buffer[5];
  length[1] = buffer[6];
  length[2] = buffer[7];
  length[3] = buffer[8];

  // if buffer contains something other than header (ie. body of msg)
  if (max_size > 9) {
    body = substr(buffer, 9, max_size - 9);
  }
}
// re-inventing the substr
char *Frame::substr(char *arr, int begin, int len) {
  char *res = new char[len + 1];
  for (int i = 0; i < len; i++) {
    res[i] = *(arr + begin + i);
  }
  res[len] = 0;
  return res;
}
