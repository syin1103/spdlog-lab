//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_FILE_SINKS_H_
#define SPDLOG_SINKS_FILE_SINKS_H_

#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
// #include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {

template <typename Mutex>
class SimpleFileSink : public BaseSink<Mutex> {};

template <typename Mutex>
class RotatingFileSink : public BaseSink<Mutex> {};

template <typename Mutex, typename FileNameCalc>
class DailyFileSink : public BaseSink<Mutex> {};

}  // namespace sinks

}  // namespace spdlog

#endif  // SPDLOG_SINKS_FILE_SINKS_H_