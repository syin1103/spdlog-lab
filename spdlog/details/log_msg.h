//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_LOG_MSG_H_
#define SPDLOG_DETAILS_LOG_MSG_H_

#include <fmt/format.h>
#include <spdlog/common.h>

#include <string>

namespace spdlog {
namespace details {

struct LogMsg {
  LogMsg() = default;
  LogMsg(const std::string* name, level::level_enum lvl)
      : logger_name(name), level(lvl) {
#ifndef SPDLOG_NO_DATETIME
    // time = os::now();
#endif

#ifndef SPDLOG_NO_THREAD_ID
    // thread_id = os::thread_id();
#endif
  }

  LogMsg(const LogMsg&) = delete;
  LogMsg& operator=(const LogMsg&) = delete;
  LogMsg(LogMsg&&) = delete;

  const std::string* logger_name;
  level::level_enum level;
  log_clock::time_point time;
  size_t thread_id;

  fmt::memory_buffer raw;
  fmt::memory_buffer formatted;
};

}  // namespace details
}  // namespace spdlog

#endif  // SPDLOG_DETAILS_LOG_MSG_H_