//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_PATTERN_FORMATTER_IMPL_H_
#define SPDLOG_DETAILS_PATTERN_FORMATTER_IMPL_H_

#include <fmt/format.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>
#include <spdlog/formatter.h>

#include <chrono>
#include <ctime>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace spdlog {
namespace details {

class FlagFormatter {
 public:
  virtual ~FlagFormatter() {}
  virtual void Format(details::LogMsg& msg, const std::tm& tm_time) = 0;
};

// name & level pattern appenders
namespace {
class NameFormatter : public FlagFormatter {
  void Format(details::LogMsg& msg, const std::tm&) override {
    fmt::format_to(std::back_inserter(msg.formatted), "{}", *msg.logger_name);
  }
};
}  // namespace

class LevelFormatter : public FlagFormatter {
  void Format(details::LogMsg& msg, const std::tm&) override {
    fmt::format_to(std::back_inserter(msg.formatted), "{}",
                   level::to_str(msg.level));
  }
};

class ShortLevelFormatter : public FlagFormatter {
  void Format(details::LogMsg& msg, const std::tm&) override {
    fmt::format_to(std::back_inserter(msg.formatted), "{}",
                   level::to_short_str(msg.level));
  }
};

// Date time pattern appenders
static const char* ampm(const tm& t) { return t.tm_hour >= 12 ? "PM" : "AM"; }

static int to12h(const tm& t) {
  return t.tm_hour > 12 ? t.tm_hour - 12 : t.tm_hour;
}

// Abbreviated weekday name
static const std::string days[]{"Sun", "Mon", "Tue", "Wed",
                                "Thu", "Fri", "Sat"};
class AFormatter : public FlagFormatter {
  void Format(details::LogMsg& msg, const std::tm& tm_time) override {
    fmt::format_to(std::back_inserter(msg.formatted), "{}",
                   days[tm_time.tm_wday]);
  }
};

// aggregate user chars to display as is
class AggregateFormatter : public FlagFormatter {
 public:
  AggregateFormatter() {}
  void add_ch(char ch) { str_ += ch; }
  void Format(details::LogMsg& msg, const std::tm&) override {
    fmt::format_to(std::back_inserter(msg.formatted), "{}", str_);
  }

 private:
  std::string str_;
};

}  // namespace details
}  // namespace spdlog

// pattern formatter inline impl

inline spdlog::PatternFormatter::PatternFormatter(const std::string& pattern) {
  CompilePattern(pattern);
}

inline void spdlog::PatternFormatter::Format(details::LogMsg& msg) {
  try {
#ifndef SPDLOG_NO_DATETIME
    auto tm_time = details::os::localtime(log_clock::to_time_t(msg.time));
#else
    std::tm tm_time{};
#endif

    for (auto& f : formatters_) {
      f->Format(msg, tm_time);
    }

    fmt::format_to(std::back_inserter(msg.formatted), "{}",
                   std::string_view(details::os::eol, details::os::eol_size));

  } catch (const fmt::format_error& e) {
    throw spdlog_ex(fmt::format(
        "formatting error while processing format string: {}", e.what()));
  }
}

inline void spdlog::PatternFormatter::HandleFlag(char flag) {}

inline void spdlog::PatternFormatter::CompilePattern(
    const std::string& pattern) {
  auto end = pattern.end();
  std::unique_ptr<details::AggregateFormatter> user_chars;
  for (auto it = pattern.begin(); it != end; it++) {
    if (*it == '%') {
      if (user_chars) {  // append user chars found so far
        formatters_.push_back(std::move(user_chars));
      }

      if ((++it != end)) {
        HandleFlag(*it);
      } else {
        break;
      }
    } else {
      // chars not following the % sign should be displayed as is
      if (!user_chars) {
        user_chars = std::unique_ptr<details::AggregateFormatter>(
            new details::AggregateFormatter());
        user_chars->add_ch((*it));
      }
    }
  }

  if (user_chars) {
    formatters_.push_back(std::move(user_chars));
  }
}

#endif  // SPDLOG_DETAILS_PATTERN_FORMATTER_IMPL_H_