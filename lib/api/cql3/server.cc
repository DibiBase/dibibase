#include "api/cql3/server.hh"
#include "api/cql3/frame.cc"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using std::cout;

using namespace dibibase::api::cql3;

Server::Server(const int port) {
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  char buffer[MAX_MESSAGE_LEN];
  memset(buffer, 0, sizeof(buffer));

  sock_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_listen_fd < 0) {
    error("Error creating socket..\n");
  }

  memset((char *)&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock_listen_fd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0)
    error("Error binding socket..\n");

  if (listen(sock_listen_fd, BACKLOG) < 0) {
    error("Error listening..\n");
  }
  printf("server listening for connections on port: %d\n", port);

  struct epoll_event events[MAX_EVENTS];
  int new_events, sock_conn_fd;

  epollfd = epoll_create(MAX_EVENTS);
  if (epollfd < 0) {
    error("Error creating epoll..\n");
  }
  ev.events = EPOLLIN;
  ev.data.fd = sock_listen_fd;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_listen_fd, &ev) == -1) {
    error("Error adding new listeding socket to epoll..\n");
  }

  while (1) {
    new_events = epoll_wait(epollfd, events, MAX_EVENTS, -1);

    if (new_events == -1) {
      error("Error in epoll_wait..\n");
    }

    for (int i = 0; i < new_events; ++i) {
      if (events[i].data.fd == sock_listen_fd) {
        sock_conn_fd = accept4(sock_listen_fd, (struct sockaddr *)&client_addr,
                               &client_len, SOCK_NONBLOCK);
        if (sock_conn_fd == -1) {
          error("Error accepting new connection..\n");
        }

        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = sock_conn_fd;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock_conn_fd, &ev) == -1) {
          error("Error adding new event to epoll..\n");
        }
      } else {
        int newsockfd = events[i].data.fd;
        int bytes_received = recv(newsockfd, buffer, MAX_MESSAGE_LEN, 0);
        if (bytes_received <= 0) {
          epoll_ctl(epollfd, EPOLL_CTL_DEL, newsockfd, NULL);
          shutdown(newsockfd, SHUT_RDWR);
        } else {
          unsigned int msg [10] = {10,10,10,01,10,10,10,10,10};
          send(newsockfd, msg , 10 , 0);
          printf("Received: ");
          for (int i = 0; i < bytes_received; i++) {
            printf("%c", buffer[i]);
          }
          printf(";\n");
          /*
          Frame f(buffer, MAX_MESSAGE_LEN);
          f.parse();
          ServerMsg m(f);
          int bytes_sent = m.CreateResponse();


          printf("Received: ");
          for (int i = 0; i < bytes_received; i++) {
            printf("%d ", buffer[i]);
          }
          printf(";\n");

          printf("SENT: ");
          for (int i = 0; i <bytes_sent; i++) {
            printf("%d ", m.Header[i]);
          }
          printf(";\n");
          send(newsockfd, m.Header , bytes_sent, 0);
          */
        }
      }
    }
  }
}
