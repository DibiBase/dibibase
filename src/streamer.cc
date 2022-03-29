#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "dht/partitioner.hh"
#include "dht/query_processor.hh"
#include "dht/state_store.hh"
#include "dht/streamer.hh"

using namespace dibibase::dht;

int main(int argc, char *argv[]) {
  std::string local_address = StateStore::instance().local_address();

  // server for internal communications
  std::thread th = std::thread(
      [local_address]() { StreamerService().run_server(local_address); });

  // external queries
  while (true) {
    std::string query;
    std::cout << ">>> " << std::flush;
    std::getline(std::cin, query);
    if (query == "") {
      continue;
    }
    std::cout << QueryProcessor(query).process() << std::endl;
  }

  // wait for server
  th.join();
}
