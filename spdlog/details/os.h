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

inline std::tm localtime(const std::time_t& time_tt) {
#ifdef _WIN32
  std::tm tm;
  localtime_s(&tm, &time_tt);
#else
  std::tm tm;
  localtime_r(&time_tt, &tm);
#endif
  return tm;
}

inline std::tm localtime() {
  std::time_t now_t = time(nullptr);
  return localtime(now_t);
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

// wchar support for windows file names (SPDLOG_WCHAR_FILENAMES must be defined)
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
#define SPDLOG_FILENAME_T(s) L##s
inline std::string filename_to_str(const filename_t& filename) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> c;
  return c.to_bytes(filename);
}
#else
#define SPDLOG_FILENAME_T(s) s
inline std::string filename_to_str(const filename_t& filename) {
  return filename;
}
#endif

inline int remove(const filename_t& filename) {
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
  return _wremove(filename.c_str());
#else
  return std::remove(filename.c_str());
#endif
}

inline int rename(const filename_t& filename1, const filename_t& filename2) {
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
  return _wrename(filename1.c_str(), filename2.c_str());
#else
  return std::rename(filename1.c_str(), filename2.c_str());
#endif
}

// fopen_s on non windows for writing
inline int fopen_s(FILE** fp, const filename_t& filename,
                   const filename_t& mode) {
#ifdef _WIN32
#ifdef SPDLOG_WCHAR_FILENAMES
  *fp = _wfsopen((filename.c_str()), mode.c_str(), _SH_DENYWR);
#else
  *fp = _fsopen((filename.c_str()), mode.c_str(), _SH_DENYWR);
#endif
  return *fp == nullptr;
#else
  *fp = fopen((filename.c_str()), mode.c_str());
  return *fp == nullptr;
#endif
}

// Return file size according to open FILE* object
inline size_t filesize(FILE* f) {
  if (f == nullptr) throw spdlog_ex("Failed getting file size. fd is null");
#ifdef _WIN32
  int fd = _fileno(f);
#if _WIN64  // 64 bits
  struct _stat64 st;
  if (_fstat64(fd, &st) == 0) return st.st_size;

#else  // windows 32 bits
  struct _stat st;
  if (_fstat(fd, &st) == 0) return st.st_size;
#endif

#else  // unix
  int fd = fileno(f);
// 64 bits(but not in osx, where fstat64 is deprecated)
#if !defined(__APPLE__) && (defined(__x86_64__) || defined(__ppc64__))
  struct stat64 st;
  if (fstat64(fd, &st) == 0) return st.st_size;
#else  // unix 32 bits or osx
  struct stat st;
  if (fstat(fd, &st) == 0) return st.st_size;
#endif
#endif
  throw spdlog_ex("Failed getting file size from fd", errno);
}

// Return if file exists
inline bool file_exists(const filename_t& filename) {
#ifdef _WIN32
#ifdef SPDLOG_WCHAR_FILENAMES
  auto attribs = GetFileAttributesW(filename.c_str());
#else
  auto attribs = GetFileAttributesA(filename.c_str());
#endif
  return (attribs != INVALID_FILE_ATTRIBUTES &&
          !(attribs & FILE_ATTRIBUTE_DIRECTORY));
#else  // common linux/unix all have the stat system call
  struct stat buffer;
  return (stat(filename.c_str(), &buffer) == 0);
#endif
}

// eol definition
#if !defined(SPDLOG_EOL)
#ifdef _WIN32
#define SPDLOG_EOL "\r\n"
#else
#define SPDLOG_EOL "\n"
#endif
#endif

constexpr static const char* eol = SPDLOG_EOL;
constexpr static int eol_size = sizeof(SPDLOG_EOL) - 1;

}  // namespace os
}  // namespace details
}  // namespace spdlog

#endif  // SPDLOG_DETAILS_OS_H_