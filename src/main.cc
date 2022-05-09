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
  if (argc < 3) {
    std::cout << "usage: " << argv[0] << " <CQL port> <PROMETHEUS port>" << std::endl;
    exit(EXIT_SUCCESS);
  }
  std::string host = "0.0.0.0";
  int port = strtol(argv[2],NULL,10);
  HttpServer prom_server(host, port);
  std::string ex_metric = "# TYPE process_max_fds gauge\nprocess_max_fds 1024.0\n";
  // Register a few endpoints for demo and benchmarking
  auto Greetings = [ex_metric](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/plain");
    response.SetContent(ex_metric);
    return response;
  };


  prom_server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, Greetings);
  prom_server.RegisterHttpRequestHandler("/", HttpMethod::GET, Greetings);


  try {

    std::cout << "Starting the web prom_server.." << std::endl;
    prom_server.Start();
    std::cout << "Server listening on " << host << ":" << port << std::endl;

    std::cout << "Enter [quit] to stop the prom_server" << std::endl;
    std::string command;
    
    using namespace dibibase::api::cql3;

    int portno = strtol(argv[1], NULL, 10);
    Server server(portno);




  } catch (std::exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}