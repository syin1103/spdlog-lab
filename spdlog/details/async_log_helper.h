//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_
#define SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
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

  ~AsyncLogHelper();

  void Log(const details::LogMsg& msg) {}
  void Flush() {}
};

}  // namespace details

}  // namespace spdlog

#endif  // SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_
