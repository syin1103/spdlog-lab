//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_
#define SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/details/mpmc_bounded_q.h>
#include <spdlog/details/os.h>
#include <spdlog/formatter.h>
#include <spdlog/sinks/sink.h>

#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace spdlog {
namespace details {

class AsyncLogHelper {
  enum class async_msg_type { log, flush, terminate };
  struct async_msg {
    std::string logger_name;
    level::level_enum level;
    log_clock::time_point time;
    size_t thread_id;
    std::string txt;
    async_msg_type msg_type;

    async_msg() = default;
    ~async_msg() = default;
    async_msg(const async_msg&) = delete;
    async_msg& operator=(const async_msg&) = delete;

    async_msg(async_msg_type type) : msg_type(type) {};
    async_msg(const details::LogMsg& msg)
        : level(msg.level),
          time(msg.time),
          thread_id(msg.thread_id),
          txt(msg.raw.data(), msg.raw.size()),
          msg_type(async_msg_type::log) {
#ifndef SPDLOG_NO_NAME
      logger_name = *msg.logger_name;
#endif
    }

    async_msg(async_msg&& other) noexcept
        : logger_name(std::move(other.logger_name)),
          level(std::move(other.level)),
          time(std::move(other.time)),
          txt(std::move(other.txt)),
          msg_type(std::move(other.msg_type)) {}

    async_msg& operator=(async_msg&& other) noexcept {
      logger_name = std::move(other.logger_name);
      level = other.level;
      time = std::move(other.time);
      thread_id = other.thread_id;
      txt = std::move(other.txt);
      msg_type = other.msg_type;
      return *this;
    }

    void fill_log_msg(LogMsg& msg) {
      msg.logger_name = &logger_name;
      msg.level = level;
      msg.time = time;
      msg.thread_id = thread_id;
      fmt::format_to(std::back_inserter(msg.raw), txt);
    }
  };

 public:
  using clock = std::chrono::steady_clock;
  AsyncLogHelper(FormatterPtr formatter, const std::vector<SinkPtr>& sinks,
                 size_t queue_size,
                 const async_overflow_policy overflow_policy =
                     async_overflow_policy::block_retry,
                 const std::function<void()>& worker_warmup_cb = nullptr,
                 const std::chrono::milliseconds& flush_interval_ms =
                     std::chrono::milliseconds::zero(),
                 const std::function<void()>& worker_teardown_cb = nullptr) {}

  ~AsyncLogHelper() {}

  void Log(const details::LogMsg& msg) {}
  void Flush() {}
  void set_formatter(FormatterPtr formatter) {}
};

}  // namespace details

}  // namespace spdlog

#endif  // SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_
