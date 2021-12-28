#pragma once

#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "common.hh"
#include "range.hh"
#include "streamer.hh"

namespace dibibase::dht {

class DIBIBASE_PUBLIC StateStore {
public:
  static std::vector<Range> get_available_ranges() {
    return std::vector<Range>{
        Range(std::numeric_limits<uint32_t>::min(),
              std::numeric_limits<uint32_t>::max() / 2, "0.0.0.0:9090"),
        Range(std::numeric_limits<uint32_t>::max() / 2,
              std::numeric_limits<uint32_t>::max(), "0.0.0.0:9091"),
    };
  }

  static std::shared_ptr<StreamerClient>
  get_streamer(const std::string address) {
    return std::shared_ptr<StreamerClient>(new StreamerClient(
        grpc::CreateChannel(address, grpc::InsecureChannelCredentials())));
  }

  static std::string get_local_address() {
    return "0.0.0.0:9090";
  }
};

} // namespace dibibase::dht