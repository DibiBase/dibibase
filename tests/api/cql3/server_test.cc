#include <iostream>

#include "api/cql3/server.hh"

int main(int argc, char **argv) {
  if (argc != 1) {
    std::cout << argv[0] << " takes no arguments.\n";
    return 1;
  }
  return 0;
}
