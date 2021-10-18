#include "util/logger.hh"

#include <unistd.h>

#include "errors.hh"
#include "util/factory.hh"

using namespace dibibase::util;

/**
 * @brief Convert string.
 */
const char *Logger::convert(string &arg) const { return arg.c_str(); }

const char *Logger::convert(const string &arg) const { return arg.c_str(); }

/**
 * @brief Create instance.
 */
Logger::make_type Logger::make(LogLevel level) {
  return *get_singleton<std::remove_reference_t<Logger::make_type>>(level);
}

/**
 * @brief Construct logger.
 */
Logger::Logger(LogLevel level) : m_level(level) {
  // clang-format off
  if (isatty(m_fd)) {
    m_prefixes[LogLevel::INFO]    = "\r\033[1;32m* \033[0m";
    m_prefixes[LogLevel::NOTICE]  = "\r\033[1;34mnotice: \033[0m";
    m_prefixes[LogLevel::WARNING] = "\r\033[1;33mwarn: \033[0m";
    m_prefixes[LogLevel::ERROR]   = "\r\033[1;31merror: \033[0m";
    m_suffixes[LogLevel::INFO]    = "\033[0m";
    m_suffixes[LogLevel::NOTICE]  = "\033[0m";
    m_suffixes[LogLevel::WARNING] = "\033[0m";
    m_suffixes[LogLevel::ERROR]   = "\033[0m";
  } else {
    m_prefixes.emplace(make_pair(LogLevel::INFO,    "dibibase|info: "));
    m_prefixes.emplace(make_pair(LogLevel::NOTICE,  "dibibase|notice: "));
    m_prefixes.emplace(make_pair(LogLevel::WARNING, "dibibase|warn: "));
    m_prefixes.emplace(make_pair(LogLevel::ERROR,   "dibibase|error: "));
    m_suffixes.emplace(make_pair(LogLevel::INFO,    ""));
    m_suffixes.emplace(make_pair(LogLevel::NOTICE,  ""));
    m_suffixes.emplace(make_pair(LogLevel::WARNING, ""));
    m_suffixes.emplace(make_pair(LogLevel::ERROR,   ""));
  }
  // clang-format on
}

/**
 * @brief Set output verbosity.
 */
void Logger::setVerbosity(LogLevel level) { m_level = level; }

/**
 * @brief Convert given LogLevel name to its enum type counterpart.
 */
LogLevel Logger::parseVerbosity(const string &name, LogLevel fallback) {
  if (name == "error") {
    return LogLevel::ERROR;
  } else if (name == "warning") {
    return LogLevel::WARNING;
  } else if (name == "notice") {
    return LogLevel::NOTICE;
  } else if (name == "info") {
    return LogLevel::INFO;
  } else {
    return fallback;
  }
}
