#pragma once

#include <filesystem>

#include "common.hh"
#include "range.hh"

#include "yaml-cpp/yaml.h"

namespace fs = std::filesystem;

namespace dibibase::config {

#define CONFIG_PATH "./config/config.yaml"

class DIBIBASE_PUBLIC Config {
public:
  static Config &instance() {
    static Config instance;
    return instance;
  }

  std::vector<Range> ranges() const { return m_ranges; };

  std::string local_address() const { return m_local_address; }

private:
  Config() {
    logger.info("loading local configurations");
    YAML::Node config = YAML::LoadFile(CONFIG_PATH);
    m_local_address = config["address"].as<std::string>();
    logger.info("local address: %s", m_local_address.c_str());

    YAML::Node config_ranges = config["ranges"];
    for (const auto &range : config_ranges) {
      uint32_t start = range["start"].as<uint32_t>();
      uint32_t end = range["end"].as<uint32_t>();
      std::string address = range["address"].as<std::string>();

      m_ranges.push_back(Range(start, end, address));

      logger.info("node %s, from: %9d, to: %9d", address.c_str(), start, end);
    }
  }

private:
  std::string m_local_address;
  std::vector<Range> m_ranges;

  Logger logger = Logger::make();
};

} // namespace dibibase::dht
