#pragma once

#include <cstdio>
#include <map>
#include <string>
#include <thread>
#include <utility>

#include "common.hh"

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

namespace dibibase::util {

using std::string;

enum class LogLevel {
  NONE = 0,
  ERROR,
  WARNING,
  NOTICE,
  INFO,
};

class DIBIBASE_PUBLIC Logger {
public:
  static const Logger &make(LogLevel level = LogLevel::INFO);

  explicit Logger(LogLevel level);

  static LogLevel parse_verbosity(const string &name,
                                  LogLevel fallback = LogLevel::INFO);

  void set_verbosity(LogLevel level);

  /**
   * @brief Output an info message.
   */
  template <typename... Args>
  void info(const string &message, Args &&...args) const {
    output(LogLevel::INFO, message, std::forward<Args>(args)...);
  }

  /**
   * @brief Output a notice.
   */
  template <typename... Args>
  void notice(const string &message, Args &&...args) const {
    output(LogLevel::NOTICE, message, std::forward<Args>(args)...);
  }

  /**
   * @brief Output a warning message.
   */
  template <typename... Args>
  void warn(const string &message, Args &&...args) const {
    output(LogLevel::WARNING, message, std::forward<Args>(args)...);
  }

  /**
   * @brief Output an error message.
   */
  template <typename... Args>
  void err(const string &message, Args &&...args) const {
    output(LogLevel::ERROR, message, std::forward<Args>(args)...);
  }

protected:
  template <typename T> decltype(auto) convert(T &&arg) const {
    return std::forward<T>(arg);
  }

  /**
   * @brief Convert string.
   */
  const char *convert(const string &arg) const;

  /**
   * @brief Write the log message to the output channel if the defined verbosity
   * level allows it.
   */
  template <typename... Args>
  void output(LogLevel level, const string &format, Args &&...values) const {
    if (level > m_level) {
      return;
    }

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif

    dprintf(
        m_fd,
        (m_prefixes.at(level) + format + m_suffixes.at(level) + "\n").c_str(),
        convert(values)...);

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
  }

private:
  LogLevel m_level{LogLevel::INFO}; // Logger verbosity level
  int m_fd{STDERR_FILENO}; // File descriptor used when writing log messages
  std::map<LogLevel, string> m_prefixes; // LogLevel specific prefixes
  std::map<LogLevel, string> m_suffixes; // LogLevel specific suffixes
};

} // namespace dibibase::util
