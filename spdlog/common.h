//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_COMMON_H_
#define SPDLOG_COMMON_H_

#include <atomic>
#include <chrono>
#include <initializer_list>
#include <memory>

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

}  // namespace spdlog

#endif  // SPDLOG_COMMON_H_