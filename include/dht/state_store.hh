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

  std::optional<std::string> get_address(const uint32_t token) {
    for (const auto &range : m_ranges) {
      if (range.contains(token)) {
        return range.address();
      }
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<StreamerClient>>
  get_streamer(const uint32_t token) {
    std::optional<std::string> address = get_address(token);
    return address.has_value()
               ? static_cast<std::optional<std::shared_ptr<StreamerClient>>>(
                     get_streamer(address.value()))
               : std::nullopt;
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
    logger.info("loading local configurations");
    YAML::Node node_config = YAML::LoadFile(LOCAL_CONFIG_FILE);
    m_local_address = node_config["address"].as<std::string>();
    logger.info("local address: %s", m_local_address.c_str());

    logger.info("loading remote configurations");
    YAML::Node remote_config = YAML::LoadFile(REMOTE_CONFIG_FILE);
    YAML::Node ranges = remote_config["ranges"];
    for (const auto &range : ranges) {
      uint32_t start = range["start"].as<uint32_t>();
      uint32_t end = range["end"].as<uint32_t>();
      std::string address = range["address"].as<std::string>();

      m_ranges.push_back(Range(start, end, address));
      m_streamers[address] = make_streamer(address);

      logger.info("node %s, from: %9d, to: %9d", address.c_str(), start, end);
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

  Logger logger = Logger::make();
};

} // namespace dibibase::dht
