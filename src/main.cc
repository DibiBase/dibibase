#include <cstdlib>
#include <iostream>

#include "api/cql3/server.hpp"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <port>" << std::endl;
    exit(EXIT_SUCCESS);
  }

  using namespace dibibase::api::cql3;

  int portno = strtol(argv[1], NULL, 10);
  Server server(portno);
}
