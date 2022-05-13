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
void cql(std::string met[] , int portno){
  met[0] = "HELLO FROM CQL";
  using namespace dibibase::api::cql3;
  Server server(portno,&met[0]);

  met[0] = "HELLO FROM CQL AFTER SERVER is called";
}
void prom(std::string met[],int port)
{
  std::string host = "0.0.0.0";
  HttpServer prom_server(host, port);

  std::string ex_metric = "# TYPE process_max_fds gauge\nprocess_max_fds 1024.0\n";
  auto Greetings = [ex_metric](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/plain");
    response.SetContent(ex_metric);
    return response;
  };
  prom_server.RegisterHttpRequestHandler("/", HttpMethod::HEAD, Greetings);
  prom_server.RegisterHttpRequestHandler("/", HttpMethod::GET, Greetings);
  std::cout << "Starting the web prom_server.." << std::endl;
  prom_server.Start();
  std::cout << "prom_server listening on " << host << ":" << port << std::endl;
  std::string in;
  while(1){
    in = met[0];
    auto Greetings2 = [in](const HttpRequest& request) -> HttpResponse {
    HttpResponse response(HttpStatusCode::Ok);
    response.SetHeader("Content-Type", "text/plain");
    response.SetContent(in);
    return response;
    };
    prom_server.RegisterHttpRequestHandler("/metrics",HttpMethod::HEAD, Greetings2);
    prom_server.RegisterHttpRequestHandler("/metrics", HttpMethod::GET, Greetings2);
    for(int j=0;j<10000;j++);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cout << "usage: " << argv[0] << " <CQL port> <PROMETHEUS port>" << std::endl;
    exit(EXIT_SUCCESS);
  }
  //should be shared among the 2 servers 
  std::string met[4] = {"MET1","MET2","MET3","MET4"};
    
  
  int portno = strtol(argv[1], NULL, 10);
  int port = strtol(argv[2],NULL,10);
  
  
  std::thread th1(prom , met , port);
  std::thread th2(cql , met , portno);

  th1.join();
  th2.join();

  return 0;
}