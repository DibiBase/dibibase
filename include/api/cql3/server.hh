#pragma once

#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>
#include <string>
#include "common.hh"
#include "api/cql3/frame.hh"
#include "api/cql3/server_response.hh"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>
#include <ctime>
#include <chrono>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "api/prom_endp/uri.hh"
#include "api/prom_endp/http_message.hh"
#include "api/prom_endp/http_server.hh"
#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

namespace dibibase::api::cql3 {

inline void error(char const *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

class DIBIBASE_PUBLIC Server {

public:
  std::string query;
  Server(int port,int prom_port);
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;
  Server(Server &&) = delete;
  Server &operator=(Server &&) = delete;
private:
  int sock_listen_fd;
  int maxevents = 128;
  int timeout = 5;
  struct epoll_event ev;
  int epollfd;
  std::string columns = "system_schema.columns";
  bool prom_started = false;
};

} // namespace dibibase::api::cql3
