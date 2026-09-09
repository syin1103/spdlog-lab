//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_COMMON_H_
#define SPDLOG_COMMON_H_

#include <atomic>
#include <chrono>
#include <exception>
#include <initializer_list>
#include <memory>
#include <string>

namespace spdlog {

// Log level enum
namespace level {

enum level_enum {
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  err = 4,
  critical = 5,
  off = 6
};

}  // namespace level

// Async overflow policy - block by default.
enum class async_overflow_policy {
  block_retry,     // Block / yield / sleep until message can be enqueued
  discard_log_msg  // Discard the message it enqueue fails
};

namespace sinks {
class Sink;
}
class Formatter;

using FormatterPtr = std::shared_ptr<spdlog::Formatter>;
using SinkPtr = std::shared_ptr<spdlog::sinks::Sink>;
using SinksInitList = std::initializer_list<SinkPtr>;

using log_clock = std::chrono::system_clock;

#if defined(SPDLOG_NO_ATOMIC_LEVELS)
using level_t = details::null_atomic_int;
#else
using level_t = std::atomic_int;
#endif

namespace details {
namespace os {
std::string errno_str(int err_num);
}
}  // namespace details

class spdlog_ex : public std::exception {
 public:
  spdlog_ex(const std::string& msg) : msg_(msg) {}
  spdlog_ex(const std::string& msg, int last_errno) {
    msg_ = msg + ": " + details::os::errno_str(last_errno);
  }
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  std::string msg_;
};

//
// wchar support for windows file names (SPDLOG_WCHAR_FILENAMES must be defined)
//
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
using filename_t = std::wstring;
#else
using filename_t = std::string;
#endif

}  // namespace spdlog

#endif  // SPDLOG_COMMON_H_