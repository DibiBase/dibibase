#pragma once

#include "common.hh"
#include "murmur_hash.hh"
#include "state_store.hh"

#include <future>

namespace dibibase::dht {

class DIBIBASE_PUBLIC Partitioner {

public:
  std::optional<std::string> get_address(const std::string &key) const {
    // uint64_t token =
    //     MurmurHash::hash2_x64((uint8_t *)key.c_str(), 0, key.size(), 0);
    uint64_t token = (atoi(key.c_str()) & 1) ? 50 : 150;
    return state_store.get_address(token);
  }

  bool is_local(const std::string &key) const {
    std::optional<std::string> address = get_address(key);
    return !address.has_value() || address == config.local_address();
  }

  string send(const string &query, const string &key) {
    std::optional<std::string> opt_address = get_address(key);

    std::string address = opt_address.value();
    auto streamer = state_store.get_streamer(address);
    logger.info("sending to %s, %s", address.c_str(), query.c_str());

    return streamer->execute(query);
  }

  void send_all(const string &query) const {
    std::vector<std::future<string>> results;
    for (const auto &[address, streamer] : state_store.available_streamers()) {
      if (address == config.local_address())
        continue;
      logger.info("sending to %s, %s", address.c_str(), query.c_str());
      results.push_back(std::async(std::launch::async,
                                   [&] { return streamer->execute(query); }));
    }

    for (auto &result : results) {
      logger.info(result.get());
    }
  }

private:
  Logger logger = Logger::make();
  Config config = Config::instance();
  StateStore state_store = StateStore::instance();
};

} // namespace dibibase::dht
