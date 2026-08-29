//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_BASE_SINK_H_
#define SPDLOG_SINKS_BASE_SINK_H_

#include <spdlog/common.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/formatter.h>
#include <spdlog/sinks/sink.h>

#include <mutex>

namespace spdlog {
namespace sinks {

template <class Mutex>
class BaseSink : public Sink {
 public:
  BaseSink() : mutex_() {}
  virtual ~BaseSink() = default;

  BaseSink(const BaseSink&) = delete;
  BaseSink& operator=(const BaseSink&) = delete;

  void Log(const details::LogMsg& msg) override {
    std::lock_guard<Mutex> lock(mutex_);
    SinkIt(msg);
  }

 protected:
  virtual void SinkIt(const details::LogMsg& msg) = 0;

  Mutex mutex_;
};

}  // namespace sinks

}  // namespace spdlog

#endif  // SPDLOG_SINKS_BASE_SINK_H_