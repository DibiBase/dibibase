#include "util/logger.hpp"

int main() {
  using namespace dibibase::util;

  Logger &logger{const_cast<decltype(logger)>(Logger::make(LogLevel::INFO))};

  logger.setVerbosity(LogLevel::INFO);

  Logger::make().warn("This is a warning!");

  logger.info("This is an info message.");
}
