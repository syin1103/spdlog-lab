//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_ASYNC_LOGGER_IMPL_H_
#define SPDLOG_DETAILS_ASYNC_LOGGER_IMPL_H_

#include <spdlog/async_logger.h>
#include <spdlog/details/async_log_helper.h>

#include <chrono>
#include <functional>
#include <memory>
#include <string>

template <class It>
inline spdlog::AsyncLogger::AsyncLogger(
    const std::string& name, const It& begin, const It& end, size_t queue_size,
    const async_overflow_policy overflow_policy,
    const std::function<void()>& worker_warmup_cb,
    const std::chrono::milliseconds& flush_interval_ms,
    const std::function<void()>& worker_teardown_cb)
    : Logger(name, begin, end),
      async_log_helper_(new details::AsyncLogHelper(
          formatter_, sinks_, queue_size, overflow_policy, worker_warmup_cb,
          flush_interval_ms, worker_teardown_cb)) {}

inline spdlog::AsyncLogger::AsyncLogger(
    const std::string& name, SinksInitList sinks, size_t queue_size,
    const async_overflow_policy overflow_policy,
    const std::function<void()>& worker_warmup_cb,
    const std::chrono::milliseconds& flush_interval_ms,
    const std::function<void()>& worker_teardown_cb)
    : AsyncLogger(name, sinks.begin(), sinks.end(), queue_size, overflow_policy,
                  worker_warmup_cb, flush_interval_ms, worker_teardown_cb) {}

inline spdlog::AsyncLogger::AsyncLogger(
    const std::string& name, SinkPtr single_sink, size_t queue_size,
    const async_overflow_policy overflow_policy,
    const std::function<void()>& worker_warmup_cb,
    const std::chrono::milliseconds& flush_interval_ms,
    const std::function<void()>& worker_teardown_cb)
    : AsyncLogger(name, {single_sink}, queue_size, overflow_policy,
                  worker_warmup_cb, flush_interval_ms, worker_teardown_cb) {}

inline void spdlog::AsyncLogger::Flush() { async_log_helper_->Flush(); }

inline void spdlog::AsyncLogger::SetFormatterInter(
    spdlog::FormatterPtr formatter) {
  formatter_ = formatter;
  async_log_helper_->set_formatter(formatter_);
}

inline void spdlog::AsyncLogger::SetPatternInter(const std::string& pattern) {
  formatter_ = std::make_shared<PatternFormatter>(pattern);
  async_log_helper_->set_formatter(formatter_);
}

inline void spdlog::AsyncLogger::SinkItInter(details::LogMsg& msg) {
  async_log_helper_->Log(msg);
}

#endif  // SPDLOG_DETAILS_ASYNC_LOGGER_IMPL_H_
