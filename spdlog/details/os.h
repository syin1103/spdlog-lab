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
#ifdef _WIN32
  return static_cast<size_t>(::GetCurrentThreadId());
#elif __linux__
#if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
#define SYS_gettid __NR_gettid
#endif
  return static_cast<size_t>(syscall(SYS_gettid));
#else  // Default to standard C++11 (OSX and other Unix)
  return static_cast<size_t>(
      std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}

inline std::string errno_str(int err_num) {
  char buf[256];
  constexpr auto buf_size = sizeof(buf);

#ifdef _WIN32
  if (strerror_s(buf, buf_size, err_num) == 0)
    return std::string(buf);
  else
    return "Unkown error";

#elif defined(__APPLE__) || \
    ((_POSIX_C_SOURCE >= 200112L) && !_GNU_SOURCE)  // posix version
  if (strerror_r(err_num, buf, buf_size) == 0)
    return std::string(buf);
  else
    return "Unkown error";

#else
  return std::string(strerror_r(err_num, buf, buf_size));
#endif
}

}  // namespace os

}  // namespace details

}  // namespace spdlog

#endif  // SPDLOG_DETAILS_OS_H_