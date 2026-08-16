//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_LOGGER_IMPL_H_
#define SPDLOG_DETAILS_LOGGER_IMPL_H_

#include <spdlog/formatter.h>
#include <spdlog/logger.h>

#include <iterator>
#include <memory>

inline spdlog::Logger::Logger(const std::string& name,
                              spdlog::SinkPtr single_sink)
    : Logger(name, {single_sink}) {}

inline spdlog::Logger::Logger(const std::string& name, SinksInitList sinks_list)
    : Logger(name, sinks_list.begin(), sinks_list.end()) {}

// all other ctors will call this one
template <class It>
inline spdlog::Logger::Logger(const std::string& name, const It& begin,
                              const It& end)
    : name_(name),
      sinks_(begin, end),
      formatter_(std::make_shared<spdlog::PatternFormatter>("%+")) {
  level_ = level::info;
  flush_level_ = level::off;
}

inline spdlog::Logger::~Logger() = default;

template <typename... Args>
inline void spdlog::Logger::Log(level::level_enum lvl, const char* fmt,
                                const Args&... args) {
  if (!ShouldLog(lvl)) {
    return;
  }
  details::LogMsg log_msg(&name_, lvl);
  try {
    // fmt::format_to(log_msg.raw, fmt, args...);
    fmt::format_to(std::back_inserter(log_msg.raw), fmt, args...);
  } catch (fmt::format_error& ex) {
    throw spdlog::spdlog_ex(std::string("format error in \"") + fmt +
                            "/: " + ex.what());
  }
  SinkItInter(log_msg);
}

template <typename... Args>
inline void spdlog::Logger::Log(level::level_enum lvl, const char* msg) {
  if (!ShouldLog(lvl)) {
    return;
  }
  details::LogMsg log_msg(&name_, lvl);
  // fmt::format_to(log_msg.raw, msg);
  fmt::format_to(std::back_inserter(log_msg.raw), "{}", msg);
  SinkItInter(log_msg);
}

template <typename... Args>
inline void spdlog::Logger::Trace(const char* fmt, const Args&... args) {
  Log(level::trace, fmt, args...);
}

template <typename... Args>
inline void spdlog::Logger::Debug(const char* fmt, const Args&... args) {
  Log(level::debug, fmt, args...);
}

template <typename... Args>
inline void spdlog::Logger::Info(const char* fmt, const Args&... args) {
  Log(level::info, fmt, args...);
}

template <typename... Args>
inline void spdlog::Logger::Warn(const char* fmt, const Args&... args) {
  Log(level::warn, fmt, args...);
}

template <typename... Args>
inline void spdlog::Logger::Error(const char* fmt, const Args&... args) {
  Log(level::err, fmt, args...);
}

template <typename... Args>
inline void spdlog::Logger::Critical(const char* fmt, const Args&... args) {
  Log(level::critical, fmt, args...);
}

template <typename T>
inline void spdlog::Logger::Log(level::level_enum lvl, const T& msg) {
  if (!ShouldLog(lvl)) {
    return;
  }
  details::LogMsg log_msg(&name_, lvl);
  // fmt::format_to(log_msg.raw, msg);
  fmt::format_to(std::back_inserter(log_msg.raw), "{}", msg);
  SinkItInter(log_msg);
}

template <typename T>
inline void spdlog::Logger::Trace(const T& msg) {
  Log(level::trace, msg);
}

template <typename T>
inline void spdlog::Logger::Debug(const T& msg) {
  Log(level::debug, msg);
}

template <typename T>
inline void spdlog::Logger::Info(const T& msg) {
  Log(level::info, msg);
}

template <typename T>
inline void spdlog::Logger::Warn(const T& msg) {
  Log(level::warn, msg);
}

template <typename T>
inline void spdlog::Logger::Error(const T& msg) {
  Log(level::err, msg);
}

template <typename T>
inline void spdlog::Logger::Critical(const T& msg) {
  Log(level::critical, msg);
}

inline bool spdlog::Logger::ShouldLog(level::level_enum msg_level) const {
  return msg_level >= level_.load(std::memory_order_relaxed);
}

inline void spdlog::Logger::set_level(spdlog::level::level_enum log_level) {
  level_.store(log_level);
}

inline spdlog::level::level_enum spdlog::Logger::level() const {
  return static_cast<spdlog::level::level_enum>(
      level_.load(std::memory_order_relaxed));
}

inline const std::string& spdlog::Logger::name() const { return name_; }

inline void spdlog::Logger::set_pattern(const std::string& pattern) {
  SetPatternInter(pattern);
}

inline void spdlog::Logger::set_formatter(FormatterPtr formatter) {
  SetFormatterInter(formatter);
}

inline void spdlog::Logger::FlushOn(spdlog::level::level_enum log_level) {
  flush_level_.store(log_level);
}

inline void spdlog::Logger::Flush() {}

inline void spdlog::Logger::SinkItInter(details::LogMsg& msg) {}

inline void spdlog::Logger::SetPatternInter(const std::string& pattern) {
  formatter_ = std::make_shared<PatternFormatter>(pattern);
}

inline void spdlog::Logger::SetFormatterInter(FormatterPtr formatter) {
  formatter_ = formatter;
}

#endif  // SPDLOG_DETAILS_LOGGER_IMPL_H_