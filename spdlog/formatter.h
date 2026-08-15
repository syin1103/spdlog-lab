//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_FORMATTER_H_
#define SPDLOG_FORMATTER_H_

#include <spdlog/details/log_msg.h>

#include <string>

namespace spdlog {

class Formatter {
 public:
  virtual ~Formatter() {}
  virtual void Format(details::LogMsg& msg) = 0;
};

class PatternFormatter : public Formatter {
 public:
  explicit PatternFormatter(const std::string& pattern);

  PatternFormatter(const PatternFormatter&) = delete;
  PatternFormatter& operator=(const PatternFormatter&) = delete;

  void Format(details::LogMsg& msg) override;

 private:
  void CompilePattern(const std::string& pattern);
};

}  // namespace spdlog

#include <spdlog/details/pattern_formatter_impl.h>

#endif  // SPDLOG_FORMATTER_H_