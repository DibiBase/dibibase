#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>


#include <cerrno>
#include <cstring>
#include <string>
#include <stdexcept>

#include "api/cql3/server.hh"
#include "api/prom_endp/uri.hh"
#include "api/prom_endp/http_message.hh"
#include "api/prom_endp/http_server.hh"

  using namespace dibibase::api::prom_endp;



int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <CQL port>" << std::endl;
    exit(EXIT_SUCCESS);
  }

  int portno = strtol(argv[1], NULL, 10);
  
  using namespace dibibase::api::cql3;
  Server server(portno);



  return 0;

}
