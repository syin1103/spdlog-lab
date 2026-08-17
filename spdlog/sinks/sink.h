//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_SINK_H_
#define SPDLOG_SINKS_SINK_H_

#include <spdlog/details/log_msg.h>

namespace spdlog {
namespace sinks {

class Sink {
 public:
  virtual ~Sink() {}
  virtual void Log(const details::LogMsg& msg) = 0;
  virtual void Flush() = 0;
};

}  // namespace sinks

}  // namespace spdlog

#endif  // SPDLOG_SINKS_SINK_H_