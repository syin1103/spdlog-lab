//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_OSTREAM_SINK_H_
#define SPDLOG_SINKS_OSTREAM_SINK_H_

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <mutex>
#include <ostream>

namespace spdlog {
namespace sinks {

template <typename Mutex>
class OstreamSink : public BaseSink<Mutex> {
 public:
  explicit OstreamSink(std::ostream& os, bool force_flush = false)
      : ostream_(os), force_flush_(force_flush) {}
  OstreamSink(const OstreamSink&) = delete;
  OstreamSink& operator=(const OstreamSink&) = delete;
  virtual ~OstreamSink() = default;

 protected:
  void SinkIt(const details::LogMsg& msg) override {
    ostream_.write(msg.formatted.data(), msg.formatted.size());
    if (force_flush_) {
      ostream_.flush();
    }
  }

  void Flush() override { ostream_.flush(); }

  std::ostream& ostream_;
  bool force_flush_;
};

typedef OstreamSink<std::mutex> OstreamSinkMt;
typedef OstreamSink<details::null_mutex> OstreamSinkSt;

}  // namespace sinks
}  // namespace spdlog

#endif  // SPDLOG_SINKS_OSTREAM_SINK_H_