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

}  // namespace details
}  // namespace spdlog

// name & level pattern appenders

inline spdlog::PatternFormatter::PatternFormatter(const std::string& pattern) {}

inline void spdlog::PatternFormatter::Format(details::LogMsg& msg) {}

inline void spdlog::PatternFormatter::CompilePattern(
    const std::string& pattern) {}

#endif  // SPDLOG_DETAILS_PATTERN_FORMATTER_IMPL_H_