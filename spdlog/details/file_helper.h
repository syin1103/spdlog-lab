//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_FILE_HELPER_H_
#define SPDLOG_DETAILS_FILE_HELPER_H_

#include <spdlog/details/log_msg.h>
#include <spdlog/details/os.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

namespace spdlog {
namespace details {

class FileHelper {
 public:
  explicit FileHelper(bool force_flush) {}

  FileHelper(const FileHelper&) = delete;
  FileHelper& operator=(const FileHelper&) = delete;

  ~FileHelper() {}

  void Open(const filename_t& filename, bool truncate = false) {}

  void Reopen(bool truncate) {}

  void Flush() {}

  void Close() {}

  void Write(const LogMsg& msg) {}

  size_t Size() { return 0; }

  const filename_t& filename() const { return filename_; }

  static bool FileExists(const filename_t& name) { return false; }

 private:
  FILE* fd_;
  filename_t filename_;
  bool force_flush_;
};

}  // namespace details
}  // namespace spdlog

#endif  // SPDLOG_DETAILS_FILE_HELPER_H_