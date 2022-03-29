#pragma once

#include "common.hh"
#include "murmur_hash.hh"
#include "query_executer.hh"
#include "state_store.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC Partitioner {

public:
  std::optional<std::string> get_address(const std::string &key) const {

    uint64_t token =
        MurmurHash::hash2_x64((uint8_t *)key.c_str(), 0, key.size(), 0);
    return StateStore::instance().get_address(token);
  }

  bool is_local(const std::string &key) const {
    std::optional<std::string> address = get_address(key);
    if (!address.has_value()) {
      return true; // key is not in any range try to insert it locally
    }
    return address == StateStore::instance().local_address();
  }

  string send(const string &query) const {

    StateStore state = StateStore::instance();
    std::string partition_key = "no"; // TODO: get partition key from query

    std::optional<std::string> opt_address = get_address(partition_key);

    if (!opt_address.has_value() || opt_address == state.local_address()) {
      return QueryExecuter(query).execute();
    }

    std::string address = opt_address.value();
    auto streamer = state.get_streamer(address);
    logger.info("sending to %s, %s", address.c_str(), query.c_str());

    return streamer->execute(query);
  }

  void send_all(const string &query) const {
    StateStore state = StateStore::instance();

    for (const auto &[address, streamer] : state.available_streamers()) {
      logger.info("sending to %s, %s", address.c_str(), query.c_str());
      streamer->execute(query);
    }
  }

private:
  Logger logger = Logger::make();
};

} // namespace dibibase::dht
