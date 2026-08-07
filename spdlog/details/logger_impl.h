//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_LOGGER_IMPL_H_
#define SPDLOG_DETAILS_LOGGER_IMPL_H_

#include <spdlog/logger.h>

inline spdlog::Logger::Logger(const std::string& name,
                              spdlog::SinkPtr single_sink) {}

inline spdlog::Logger::Logger(const std::string& name,
                              SinksInitList sinks_list) {}

template <class It>
inline spdlog::Logger::Logger(const std::string& name, const It& begin,
                              const It& end) {}

inline spdlog::Logger::~Logger() = default;

template <typename... Args>
inline void spdlog::Logger::Log(level::level_enum lvl, const char* fmt,
                                const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Log(level::level_enum lvl, const char* msg) {}

template <typename... Args>
inline void spdlog::Logger::Trace(const char* fmt, const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Debug(const char* fmt, const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Info(const char* fmt, const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Warn(const char* fmt, const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Error(const char* fmt, const Args&... args) {}

template <typename... Args>
inline void spdlog::Logger::Critical(const char* fmt, const Args&... args) {}

inline bool spdlog::Logger::ShouldLog(level::level_enum msg_level) const {
  return true;
}

inline void spdlog::Logger::set_level(spdlog::level::level_enum log_level) {}

inline spdlog::level::level_enum spdlog::Logger::level() const {
  return level::level_enum::info;
}

inline const std::string& spdlog::Logger::name() const { return ""; }

inline void spdlog::Logger::set_pattern(const std::string& pattern) {}
inline void spdlog::Logger::set_formatter(FormatterPtr formatter) {}

inline void spdlog::Logger::FlushOn(spdlog::level::level_enum log_level) {}

inline void spdlog::Logger::Flush() {}

#endif  // SPDLOG_DETAILS_LOGGER_IMPL_H_