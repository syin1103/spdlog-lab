//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_SINKS_FILE_SINKS_H_
#define SPDLOG_SINKS_FILE_SINKS_H_

#include <fmt/format.h>
#include <spdlog/details/file_helper.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <mutex>
#include <string>

namespace spdlog {
namespace sinks {

/*
 * Trivial file sink with single file as target
 */
template <typename Mutex>
class SimpleFileSink : public BaseSink<Mutex> {
 public:
  explicit SimpleFileSink(const filename_t& filename, bool force_flush = false)
      : file_helper_(force_flush) {
    file_helper_.Open(filename);
  }

  void Flush() override { file_helper_.Flush(); }

 protected:
  void SinkIt(const details::LogMsg& msg) override { file_helper_.Write(msg); }

 private:
  details::FileHelper file_helper_;
};

typedef SimpleFileSink<std::mutex> SimpleFileSinkMt;
typedef SimpleFileSink<details::null_mutex> SimpleFileSinkSt;

template <typename Mutex>
class RotatingFileSink : public BaseSink<Mutex> {
 public:
  RotatingFileSink(const filename_t& base_filename, const filename_t& extension,
                   std::size_t max_size, std::size_t max_files,
                   bool force_flush = false)
      : base_filename_(base_filename),
        extension_(extension),
        max_size_(max_size),
        max_files_(max_files),
        current_size_(0),
        file_helper_(force_flush) {}

  void Flush() override { file_helper_.Flush(); }

 protected:
  void SinkIt(const details::LogMsg& msg) override {
    current_size_ += msg.formatted.size();
    if (current_size_ > max_size_) {
      Rotate();
      current_size_ = msg.formatted.size();
    }
    file_helper_.Write(msg);
  }

 private:
  static std::string calc_filename(const std::string& filename,
                                   std::size_t index,
                                   const std::string& extension) {
    if (index) {
      return fmt::format("{}.{}.{}", filename, index, extension);
    } else {
      return fmt::format("{}.{}", filename, extension);
    }
  }

  // Rotate files:
  // log.txt -> log.1.txt
  // log.1.txt -> log2.txt
  // log.2.txt -> log3.txt
  // log.3.txt -> delete

  void Rotate() {
    using details::os::filename_to_str;
    file_helper_.Close();
    for (auto i = max_files_; i > 0; i--) {
      filename_t src = calc_filename(base_filename_, i - 1, extension_);
      filename_t target = calc_filename(base_filename_, i, extension_);

      if (details::FileHelper::FileExists(target)) {
        if (details::os::remove(target) != 0) {
          throw spdlog_ex(
              "rotating_file_sink: failed removeing " + filename_to_str(target),
              errno);
        }
      }

      if (details::FileHelper::FileExists(src) &&
          details::os::rename(src, target)) {
        throw spdlog_ex("rotating_file_sink: failed renaming " +
                            filename_to_str(src) + "to" +
                            filename_to_str(target),
                        errno);
      }
    }
    file_helper_.Reopen(true);
  }

  filename_t base_filename_;
  filename_t extension_;
  std::size_t max_size_;
  std::size_t max_files_;
  std::size_t current_size_;
  details::FileHelper file_helper_;
};

typedef RotatingFileSink<std::mutex> RotatingFileSinkMt;
typedef RotatingFileSink<details::null_mutex> RotatingFileSinkSt;

/*
 * Default generator of daily log file names.
 */
struct DefaultDailyFileNameCalculator {
  // Create filename for the form basename.YYYY-MM-DD_hh-mm.extension
  static filename_t CalcFileName(const filename_t& basename,
                                 const filename_t& extension) {
    std::tm tm = spdlog::details::os::localtime();
    return fmt::format(
        SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}_{:02d}-{:02d}.{}"), basename,
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
        extension);
  }
};

/*
 * Generator of daily log file names in format basename.YYYY-MM-DD.extension
 */
struct DateOnlyDailyFileNameCalculator {
  static filename_t CalcFileName(const filename_t& basename,
                                 const filename_t& extension) {
    std::tm tm = spdlog::details::os::localtime();
    return fmt::format(SPDLOG_FILENAME_T("{}_{:04d}-{:02d}-{:02d}.{}"),
                       basename, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                       extension);
  }
};

/*
 * Rotating file sink based on date. rotates at midnight
 */
template <typename Mutex,
          typename FileNameCalc = DefaultDailyFileNameCalculator>
class DailyFileSink : public BaseSink<Mutex> {};

typedef DailyFileSink<std::mutex> DailyFileSinkMt;
typedef DailyFileSink<details::null_mutex> DailyFileSinkSt;

}  // namespace sinks
}  // namespace spdlog

#endif  // SPDLOG_SINKS_FILE_SINKS_H_