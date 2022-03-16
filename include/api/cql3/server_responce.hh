#pragma once

#include "api/cql3/frame.hh"
#include "common.hh"
#include <arpa/inet.h>
#include <cerrno>
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

class DIBIBASE_PUBLIC ServerMsg {
public:
  ServerMsg(Frame f);
  int SupportedMessage(std::map<std::string, std::list<std::string>>,
                       unsigned char *);
  int CreateResponse(int);
  std::string query;
  unsigned char Header[71680];

private:
  enum flag { NONE = 0x00, COMPRESSED = 0x01, TRACING = 0x02 };
  enum Opcode {
    ERROR = 0x00,          /**< enum value ERROR. */
    STARTUP = 0x01,        /**< enum value STARTUP. */
    READY = 0x02,          /**< enum value READY. */
    AUTHENTICATE = 0x03,   /**< enum value AUTHENTICATE. */
    OPTIONS = 0x05,        /**< enum value OPTIONS. */
    SUPPORTED = 0x06,      /**< enum value SUPPORTED. */
    QUERY = 0x07,          /**< enum value QUERY. */
    RESULT = 0x08,         /**< enum value RESULT. */
    PREPARE = 0x09,        /**< enum value PREPARE. */
    EXECUTE = 0x0A,        /**< enum value EXECUTE. */
    REGISTER = 0x0B,       /**< enum value REGISTER. */
    EVENT = 0x0C,          /**< enum value EVENT. */
    BATCH = 0x0D,          /**< enum value BATCH. */
    AUTH_CHALLENGE = 0x0E, /**< enum value AUTH_CHALLENGE. */
    AUTH_RESPONSE = 0x0F,  /**< enum value AUTH_RESPONSE. */
    AUTH_SUCCESS = 0x10    /**< enum value AUTH_SUCCESS. */
  };
  enum Version { VERSION = 0x83, KEY = 0x83 };
  Frame v() {
    char q[21] = {'0', '3', '0', '0', '0', '0', '0', '0', '0',
                  '5', '0', '0', '0', '0', '0', '2', '3', '4'}; // ex frame
    Frame framee(q, sizeof(q));
    return framee;
  }
  Frame frame = v();
};

} // namespace dibibase::api::cql3
