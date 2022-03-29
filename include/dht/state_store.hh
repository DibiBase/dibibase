#pragma once

#include "common.hh"
#include "range.hh"

#include "streamer.hh"

#include "yaml-cpp/yaml.h"

namespace dibibase::dht {

#define LOCAL_CONFIG_FILE "./config/node.yaml"
#define REMOTE_CONFIG_FILE "./config/remote.yaml"

class DIBIBASE_PUBLIC StateStore {
public:
  static StateStore &instance() {
    static StateStore instance;
    return instance;
  }

  std::vector<Range> available_ranges() { return m_ranges; };

  std::map<std::string, std::shared_ptr<StreamerClient>> available_streamers() {
    return m_streamers;
  };

  std::string local_address() { return m_local_address; }

  std::shared_ptr<StreamerClient> get_streamer(const uint32_t token) {
    for (const auto &range : m_ranges) {
      if (range.contains(token)) {
        return get_streamer(range.address());
      }
    }
    return nullptr;
  }

  std::shared_ptr<StreamerClient> get_streamer(const std::string &address) {
    auto it = m_streamers.find(address);
    if (it == m_streamers.end()) {
      auto streamer = make_streamer(address);
      m_streamers[address] = streamer;
      return streamer;
    }
    return it->second;
  }

private:
  StateStore() {
    YAML::Node node_config = YAML::LoadFile(LOCAL_CONFIG_FILE);
    m_local_address = node_config["address"].as<std::string>();

    YAML::Node remote_config = YAML::LoadFile(REMOTE_CONFIG_FILE);
    YAML::Node ranges = remote_config["ranges"];
    for (const auto &range : ranges) {
      uint32_t start = range["start"].as<uint32_t>();
      uint32_t end = range["end"].as<uint32_t>();
      std::string address = range["address"].as<std::string>();

      m_ranges.push_back(Range(start, end, address));
      m_streamers[address] = make_streamer(address);
    }
  }

  std::shared_ptr<StreamerClient> make_streamer(const std::string &address) {
    return std::make_shared<StreamerClient>(
        ::grpc::CreateChannel(address, ::grpc::InsecureChannelCredentials()));
  }

private:
  std::map<std::string, std::shared_ptr<StreamerClient>> m_streamers;
  std::string m_local_address;
  std::vector<Range> m_ranges;
};

} // namespace dibibase::dht
