//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_ASYNC_LOGGER_H_
#define SPDLOG_ASYNC_LOGGER_H_

#include <spdlog/common.h>
#include <spdlog/logger.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace spdlog {

namespace details {
class AsyncLogHelper;
}

class AsyncLogger : public Logger {
 public:
  template <class It>
  AsyncLogger(const std::string& name, const It& begin, const It& end,
              size_t queue_size,
              const async_overflow_policy overflow_policy =
                  async_overflow_policy::block_retry,
              const std::function<void()>& worker_warmup_cb = nullptr,
              const std::chrono::milliseconds& flush_interval_ms =
                  std::chrono::milliseconds::zero(),
              const std::function<void()>& worker_teardown_cb = nullptr);

  AsyncLogger(const std::string& name, SinksInitList sinks, size_t queue_size,
              const async_overflow_policy overflow_policy =
                  async_overflow_policy::block_retry,
              const std::function<void()>& worker_warmup_cb = nullptr,
              const std::chrono::milliseconds& flush_interval_ms =
                  std::chrono::milliseconds::zero(),
              const std::function<void()>& worker_teardown_cb = nullptr);

  AsyncLogger(const std::string& name, SinkPtr single_sink, size_t queue_size,
              const async_overflow_policy overflow_policy =
                  async_overflow_policy::block_retry,
              const std::function<void()>& worker_warmup_cb = nullptr,
              const std::chrono::milliseconds& flush_interval_ms =
                  std::chrono::milliseconds::zero(),
              const std::function<void()>& worker_teardown_cb = nullptr);

  void Flush() override;

 protected:
  void SinkItInter(details::LogMsg& msg) override;
  void SetFormatterInter(spdlog::FormatterPtr formatter) override;
  void SetPatternInter(const std::string& pattern) override;

 private:
  std::unique_ptr<details::AsyncLogHelper> async_log_helper_;
};

}  // namespace spdlog

#include <spdlog/details/async_logger_impl.h>

#endif  // SPDLOG_ASYNC_LOGGER_H_