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
  using item_type = async_msg;
  using q_type = details::mpmc_bounded_queue<item_type>;
  using clock = std::chrono::steady_clock;

  AsyncLogHelper(FormatterPtr formatter, const std::vector<SinkPtr>& sinks,
                 size_t queue_size,
                 const async_overflow_policy overflow_policy =
                     async_overflow_policy::block_retry,
                 const std::function<void()>& worker_warmup_cb = nullptr,
                 const std::chrono::milliseconds& flush_interval_ms =
                     std::chrono::milliseconds::zero(),
                 const std::function<void()>& worker_teardown_cb = nullptr);

  ~AsyncLogHelper();

  void Log(const details::LogMsg& msg);
  void Flush();
  void set_formatter(FormatterPtr formatter);

 private:
  void PushMsg(async_msg&& new_msg);
  void ThrowIfBadWorker();
  void WorkerLoop();
  bool ProcessNextMsg(log_clock::time_point& last_pop,
                      log_clock::time_point& last_flush);
  void HandleFlushInterval(log_clock::time_point& now,
                           log_clock::time_point& last_flush);

  static void SleepOrYield(const log_clock::time_point& now,
                           const log_clock::time_point& last_op_time);

  FormatterPtr formatter_;
  std::vector<std::shared_ptr<sinks::Sink>> sinks_;
  q_type q_;

  bool flush_requested_;
  bool terminate_requested_;

  std::shared_ptr<spdlog_ex> last_workerthread_ex_;
  const async_overflow_policy overflow_policy_;
  const std::function<void()> worker_warmup_cb_;
  const std::function<void()> worker_teardown_cb_;
  const std::chrono::milliseconds flush_interval_ms_;
  std::thread worker_thread_;
};

}  // namespace details

}  // namespace spdlog

inline spdlog::details::AsyncLogHelper::AsyncLogHelper(
    FormatterPtr formatter, const std::vector<SinkPtr>& sinks,
    size_t queue_size, const async_overflow_policy overflow_policy,
    const std::function<void()>& worker_warmup_cb,
    const std::chrono::milliseconds& flush_interval_ms,
    const std::function<void()>& worker_teardown_cb)
    : formatter_(formatter),
      sinks_(sinks),
      q_(queue_size),
      flush_requested_(false),
      terminate_requested_(false),
      overflow_policy_(overflow_policy),
      worker_warmup_cb_(worker_warmup_cb),
      flush_interval_ms_(flush_interval_ms),
      worker_teardown_cb_(worker_teardown_cb),
      worker_thread_(&AsyncLogHelper::WorkerLoop, this) {}

inline spdlog::details::AsyncLogHelper::~AsyncLogHelper() {
  try {
    PushMsg(async_msg(async_msg_type::terminate));
    worker_thread_.join();
  } catch (...) {
    // don't crash in destructor
  }
}

inline void spdlog::details::AsyncLogHelper::Log(const details::LogMsg& msg) {
  PushMsg(async_msg(msg));
}

inline void spdlog::details::AsyncLogHelper::PushMsg(
    details::AsyncLogHelper::async_msg&& new_msg) {
  ThrowIfBadWorker();
  if (!q_.enqueue(std::move(new_msg)) &&
      overflow_policy_ != async_overflow_policy::discard_log_msg) {
    auto last_op_time = details::os::now();
    auto now = last_op_time;
    do {
      now = details::os::now();
      SleepOrYield(now, last_op_time);
    } while (!q_.enqueue(std::move(new_msg)));
  }
}

inline void spdlog::details::AsyncLogHelper::Flush() {
  PushMsg(async_msg(async_msg_type::flush));
}

inline void spdlog::details::AsyncLogHelper::WorkerLoop() {
  try {
    if (worker_warmup_cb_) {
      worker_warmup_cb_();
    }
    auto last_pop = details::os::now();
    auto last_flush = last_pop;
    while (ProcessNextMsg(last_pop, last_flush));
    if (worker_teardown_cb_) {
      worker_teardown_cb_();
    }
  } catch (const std::exception& ex) {
    last_workerthread_ex_ = std::make_shared<spdlog_ex>(
        std::string("async_logger worker thread exception: ") + ex.what());
  } catch (...) {
    last_workerthread_ex_ =
        std::make_shared<spdlog_ex>("async_logger worker thread exception");
  }
}

inline bool spdlog::details::AsyncLogHelper::ProcessNextMsg(
    log_clock::time_point& last_pop, log_clock::time_point& last_flush) {
  async_msg incoming_async_msg;
  if (q_.dequeue(incoming_async_msg)) {
    last_pop = details::os::now();
    switch (incoming_async_msg.msg_type) {
      case async_msg_type::flush:
        flush_requested_ = true;
        break;
      case async_msg_type::terminate:
        flush_requested_ = true;
        terminate_requested_ = true;
        break;
      default:
        log_msg incoming_log_msg;
        incoming_async_msg.fill_log_msg(incoming_log_msg);
        formatter_->Format(incoming_log_msg);
        for (auto& s : sinks_) {
          s->Log(incoming_log_msg);
        }
    }
    return true;
  } else {
    auto now = details::os::now();
    HandleFlushInterval(now, last_flush);
    SleepOrYield(now, last_pop);
    return !terminate_requested_;
  }

  return true;
}

inline void spdlog::details::AsyncLogHelper::HandleFlushInterval(
    log_clock::time_point& now, log_clock::time_point& last_flush) {}

inline void spdlog::details::AsyncLogHelper::set_formatter(
    FormatterPtr formatter) {
  formatter_ = formatter;
}

inline void spdlog::details::AsyncLogHelper::SleepOrYield(
    const spdlog::log_clock::time_point& now,
    const spdlog::log_clock::time_point& last_op_time) {}

inline void spdlog::details::AsyncLogHelper::ThrowIfBadWorker() {
  if (last_workerthread_ex_) {
    auto ex = std::move(last_workerthread_ex_);
    throw *ex;
  }
}

#endif  // SPDLOG_DETAILS_ASYNC_LOG_HELPER_H_
