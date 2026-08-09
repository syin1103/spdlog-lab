//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_OS_H_
#define SPDLOG_DETAILS_OS_H_

#include <spdlog/common.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <cstdio>
#include <ctime>
#include <functional>
#include <string>
#include <thread>

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX  // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifdef __MINGW32__
#include <share.h>
#endif

#include <sys/types.h>

#elif __linux__

#include <sys/syscall.h>  //Use gettid() syscall under linux to get thread id
#include <unistd.h>

#include <chrono>

#endif

namespace spdlog {
namespace details {
namespace os {

inline spdlog::log_clock::time_point now() {
#if defined __linux__ && defined SPDLOG_CLOCK_COARSE
  timespec ts;
  ::clock_gettime(CLOCK_REALTIME_COARSE, &ts);
  return std::chrono::time_point<log_clock, typename log_clock::duration>(
      std::chrono::duration_cast<typename log_clock::duration>(
          std::chrono::seconds(ts.tv_sec) +
          std::chrono::nanoseconds(ts.tv_nsec));)
#else
  return log_clock::now();
#endif
}

inline size_t thread_id() {
  // TODO(syin1103)
  return static_cast<size_t>(
      std::hash<std::thread::id>()(std::this_thread::get_id()));
}

}  // namespace os

}  // namespace details

}  // namespace spdlog

#endif  // SPDLOG_DETAILS_OS_H_