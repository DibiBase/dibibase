#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "common.hh"
#include "state_store.hh"
#include "murmur_hash.hh"

#include "streamer.pb.h"
#include "streamer.grpc.pb.h"

using dibibase::grpc::Query;

namespace dibibase::dht {

class DIBIBASE_PUBLIC Partitioner {

public:
  uint64_t get_token(const std::string &key) const {
    return MurmurHash::hash2_64((uint8_t *)key.c_str(), 0, key.size(), 0);
  }

  uint64_t send_query(const Query& query) const {
    std::string partition_key = query.predicates[0].value();
    uint64_t token = get_token(partition_key);
    std::string address = get_address(token);

    if (address == StateStore::get_local_address()) {
      return m_query_processor->execute(&query);
    }

    auto streamer = StateStore::get_streamer(address);
    return streamer->execute(&query);
  }

private:
  std::string get_address(uint64_t token) const {
    for(const auto &range : StateStore::get_available_ranges()) {
      if(range.contains(token)) {
        return range.address();
      }
    }
    return "";
  }


  std::shared_ptr<QueryProcessor> m_query_processor;
};

} // namespace dibibase::dht