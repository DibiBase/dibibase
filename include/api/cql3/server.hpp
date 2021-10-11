#pragma once

#include <cstdio>
#include <cstdlib>
#include <sys/epoll.h>

#include "common.hpp"

#define BACKLOG 512
#define MAX_EVENTS 128
#define MAX_MESSAGE_LEN 2048

namespace dibibase {
namespace api {
namespace cql3 {

inline void error(char const *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

class DIBIBASE_PUBLIC Server {

public:
  Server(int port);
  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;
  Server(Server &&) = delete;
  Server &operator=(Server &&) = delete;

private:
  int sock_listen_fd;
  struct epoll_event ev;
  int epollfd;
};

} // namespace cql3
} // namespace api
} // namespace dibibase
