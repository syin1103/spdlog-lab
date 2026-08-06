//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_LOGGER_H_
#define SPDLOG_LOGGER_H_

#include <spdlog/common.h>

#include <string>
#include <vector>

namespace spdlog {

class Logger {
 public:
  Logger(const std::string& name, SinkPtr single_sink);
  Logger(const std::string& name, SinksInitList sinks_list);

  template <class It>
  Logger(const std::string& name, const It& begin, const It& end);

  virtual ~Logger();
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

 protected:
  const std::string name_;
  std::vector<SinkPtr> sinks_;
  FormatterPtr formatter_;
  spdlog::level_t level_;
  spdlog::level_t flush_level_;
};

}  // namespace spdlog

#include "spdlog/details/logger_impl.h"

#endif  // SPDLOG_LOGGER_H_