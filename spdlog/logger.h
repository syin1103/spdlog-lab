//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_LOGGER_H_
#define SPDLOG_LOGGER_H_

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>

#include <string>
#include <vector>

namespace spdlog {

class Logger {
 public:
  Logger(const std::string& name, SinkPtr single_sink);
  Logger(const std::string& name, SinksInitList sinks_list);

  template <class It>
  Logger(const std::string& name, const It& begin, const It& end);

  virtual ~Logger();
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  template <typename... Args>
  void Log(level::level_enum lvl, const char* fmt, const Args&... args);

  template <typename... Args>
  void Log(level::level_enum lvl, const char* msg);

  template <typename... Args>
  void Trace(const char* fmt, const Args&... args);

  template <typename... Args>
  void Debug(const char* fmt, const Args&... args);

  template <typename... Args>
  void Info(const char* fmt, const Args&... args);

  template <typename... Args>
  void Warn(const char* fmt, const Args&... args);

  template <typename... Args>
  void Error(const char* fmt, const Args&... args);

  template <typename... Args>
  void Critical(const char* fmt, const Args&... args);

  template <typename T>
  void Log(level::level_enum lvl, const T& msg);

  template <typename T>
  void Trace(const T& msg);

  template <typename T>
  void Debug(const T& msg);

  template <typename T>
  void Info(const T& msg);

  template <typename T>
  void Warn(const T& msg);

  template <typename T>
  void Error(const T& msg);

  template <typename T>
  void Critical(const T& msg);

  bool ShouldLog(level::level_enum msg_level) const;

  void set_level(level::level_enum log_level);
  level::level_enum level() const;

  const std::string& name() const;
  void set_pattern(const std::string& pattern);
  void set_formatter(FormatterPtr formatter);

  void FlushOn(level::level_enum log_level);

  virtual void Flush();

 protected:
  virtual void SinkItInter(details::LogMsg& msg);
  virtual void SetPatternInter(const std::string& patter);
  virtual void SetFormatterInter(FormatterPtr formatter);

  const std::string name_;
  std::vector<SinkPtr> sinks_;
  FormatterPtr formatter_;
  spdlog::level_t level_;
  spdlog::level_t flush_level_;
};

}  // namespace spdlog

#include <spdlog/details/logger_impl.h>

#endif  // SPDLOG_LOGGER_H_