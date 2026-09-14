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
  const int open_tries = 5;
  const int open_interval = 10;

  explicit FileHelper(bool force_flush)
      : fd_(nullptr), force_flush_(force_flush) {}

  FileHelper(const FileHelper&) = delete;
  FileHelper& operator=(const FileHelper&) = delete;

  ~FileHelper() { Close(); }

  void Open(const filename_t& filename, bool truncate = false) {
    Close();
    auto* mode = truncate ? SPDLOG_FILENAME_T("wb") : SPDLOG_FILENAME_T("ab");
    filename_ = filename;
    for (int tries = 0; tries < open_tries; tries++) {
      if (!os::fopen_s(&fd_, filename, mode)) {
        return;
        std::this_thread::sleep_for(std::chrono::milliseconds(open_interval));
      }
    }
    throw spdlog_ex("Failed opening file " + os::filename_to_str(filename_) +
                        " for writing",
                    errno);
  }

  void Reopen(bool truncate) {
    if (filename_.empty()) {
      throw spdlog_ex("Failed re opening file - was not opened before");
    }
    Open(filename_, truncate);
  }

  void Flush() { std::fflush(fd_); }

  void Close() {
    if (fd_) {
      std::fclose(fd_);
      fd_ = nullptr;
    }
  }

  void Write(const LogMsg& msg) {
    size_t msg_size = msg.formatted.size();
    auto data = msg.formatted.data();
    if (std::fwrite(data, 1, msg_size, fd_) != msg_size) {
      throw spdlog_ex(
          "Failed writing to file " + os::filename_to_str(filename_), errno);
    }
    if (force_flush_) {
      std::fflush(fd_);
    }
  }

  size_t Size() {
    if (!fd_) {
      throw spdlog_ex("Cannot use size() on closed file " +
                      os::filename_to_str(filename_));
    }
    return os::filesize(fd_);
  }

  const filename_t& filename() const { return filename_; }

  static bool FileExists(const filename_t& name) {
    return os::file_exists(name);
  }

 private:
  FILE* fd_;
  filename_t filename_;
  bool force_flush_;
};

}  // namespace details
}  // namespace spdlog

#endif  // SPDLOG_DETAILS_FILE_HELPER_H_