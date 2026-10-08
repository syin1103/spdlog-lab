//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_STDOUT_SINK_H_
#define SPDLOG_SINKS_STDOUT_SINK_H_

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <cstdio>
#include <memory>
#include <mutex>

namespace spdlog {
namespace sinks {

template <typename Mutex>
class StdoutSink : public BaseSink<Mutex> {
  using MyType = StdoutSink<Mutex>;

 public:
  static std::shared_ptr<MyType> instance() {
    static std::shared_ptr<MyType> instance = std::make_shared<MyType>();
    return instance;
  }

  void Flush() override { fflush(stdout); }

 protected:
  void SinkIt(const details::LogMsg& msg) override {
    fwrite(msg.formatted.data(), sizeof(char), msg.formatted.size(), stdout);
    Flush();
  }

 private:
  StdoutSink() {}
};

}  // namespace sinks
}  // namespace spdlog

#endif  // SPDLOG_SINKS_STDOUT_SINK_H_