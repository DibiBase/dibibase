#include "util/logger.hh"

int main() {
  using namespace dibibase::util;

  Logger logger = Logger::make();

  logger.info("This is an info message.");
  logger.notice("This is an notice message.");
  logger.warn("This is an warning message.");
  logger.err("This is an error message.");

  logger.set_verbosity(LogLevel::WARNING);

  logger.info("This should not appear.");
  logger.notice("This should not appear.");
  logger.warn("This should appear as warning.");
  logger.err("This should appear as error.");

  logger.set_verbosity(LogLevel::NONE);

  logger.info("This should not appear.");
  logger.notice("This should not appear.");
  logger.warn("This should not appear.");
  logger.err("This should not appear.");
}
