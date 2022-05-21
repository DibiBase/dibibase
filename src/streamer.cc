#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "config/config.hh"
#include "dht/partitioner.hh"
#include "dht/query_processor.hh"
#include "dht/state_store.hh"
#include "dht/streamer/service.hh"

using namespace dibibase::dht;

int main(int argc, char *argv[]) {
  std::string local_address = Config::instance().local_address();

  // server for internal communications
  std::thread th = std::thread(
      [local_address]() { StreamerService().run_server(local_address); });

  // external queries
  std::ifstream stream;
  std::string query;
  stream.open(argv[1]);

  while (!stream.eof()) {
    std::cout << ">>> ";
    std::getline(std::cin, query);
    // std::cout << query << std::endl;
    if (query == "") {
      continue;
    }
    string result = QueryProcessor(query).process();
    std::cout << "Result: " << result << std::endl;
  }

  // wait for server
  th.join();
}
