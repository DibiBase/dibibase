#pragma once

#include "common.hh"

#include "config/config.hh"
#include "db/database.hh"
#include "streamer/client.hh"

#include "yaml-cpp/yaml.h"

using namespace dibibase::db;
using namespace dibibase::config;

namespace dibibase::dht {

class DIBIBASE_PUBLIC StateStore {
public:
  static StateStore &instance() {
    static StateStore instance;
    return instance;
  }

  std::map<std::string, std::shared_ptr<StreamerClient>> available_streamers() const {
    return m_streamers;
  };

  std::optional<std::string> get_address(const uint32_t token) const {
    for (const auto &range : config.ranges()) {
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

  std::shared_ptr<Database> get_keyspace(const std::string &keyspace)  {
    auto it = m_keyspaces.find(keyspace);
    if (it == m_keyspaces.end()) {
      auto keyspace_ptr = make_keyspace(keyspace);
      m_keyspaces[keyspace] = keyspace_ptr;
      return keyspace_ptr;
    }
    return it->second;
  }

private:
  StateStore() {
    for (const auto &range : config.ranges()) {
      m_streamers[range.address()] = make_streamer(range.address());
    }
  }

  std::shared_ptr<StreamerClient> make_streamer(const std::string &address) {
    return std::make_shared<StreamerClient>(
        ::grpc::CreateChannel(address, ::grpc::InsecureChannelCredentials()));
  }

  std::shared_ptr<Database> make_keyspace(const std::string &keyspace) {
    return std::make_shared<Database>(keyspace);
  }

private:
  std::map<std::string, std::shared_ptr<StreamerClient>> m_streamers;
  std::map<std::string, std::shared_ptr<Database>> m_keyspaces;

  Config config = Config::instance();
  Logger logger = Logger::make();
};

} // namespace dibibase::dht
