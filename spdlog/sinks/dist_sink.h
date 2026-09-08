//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_DIST_SINK_H_
#define SPDLOG_SINKS_DIST_SINK_H_

#include <spdlog/details/log_msg.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/sink.h>

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

namespace spdlog {
namespace sinks {

template <typename Mutex>
class DistSink : public BaseSink<Mutex> {
 public:
  explicit DistSink() : sinks_() {}

  DistSink(const DistSink&) = delete;
  DistSink& operator=(const DistSink&) = delete;
  virtual ~DistSink() = default;

  void Flush() override {
    std::lock_guard<Mutex> lock(BaseSink<Mutex>::mutex_);
    for (auto iter = sinks_.begin(); iter != sinks_.end(); iter++) {
      (*iter)->Flush();
    }
  }

  void AddSink(std::shared_ptr<Sink> sink) {
    std::lock_guard<Mutex> lock(BaseSink<Mutex>::mutex_);
    if (sink && sinks_.end() == std::find(sinks_.begin(), sinks_.end(), sink)) {
      sinks_.push_back(sink);
    }
  }

  void RemoveSink(std::shared_ptr<Sink> sink) {
    std::lock_guard<Mutex> lock(BaseSink<Mutex>::mutex_);
    auto pos = std::find(sinks_.begin(), sinks_.end(), sink);
    if (pos != sinks_.end()) {
      sinks_.erase(pos);
    }
  }

 protected:
  void SinkIt(const details::LogMsg& msg) override {
    for (auto iter = sinks_.begin(); iter != sinks_.end(); iter++) {
      (*iter)->Log(msg);
    }
  }
  std::vector<std::shared_ptr<Sink>> sinks_;
};

typedef DistSink<std::mutex> DistSinkMt;
typedef DistSink<details::null_mutex> DistSinkSt;

}  // namespace sinks
}  // namespace spdlog

#endif  // SPDLOG_SINKS_DIST_SINK_H_