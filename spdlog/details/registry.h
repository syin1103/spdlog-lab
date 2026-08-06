//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_REGISTRY_H_
#define SPDLOG_DETAILS_REGISTRY_H_

#include <spdlog/common.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/logger.h>

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace spdlog {

template <class Mutex>
class RegistryT {
 public:
  void RegisterLogger(std::shared_ptr<Logger> logger) {}

  std::shared_ptr<Logger> Get(const std::string& logger_name) {
    return nullptr;
  }

  template <class It>
  std::shared_ptr<Logger> Create(const std::string& logger_name,
                                 const It& sinks_begin, const It& sinks_end) {
    return nullptr;
  }

  std::shared_ptr<Logger> Create(const std::string& logger_name,
                                 SinksInitList) {
    return nullptr;
  }

  std::shared_ptr<Logger> Create(const std::string& logger_name, SinkPtr sink) {
    return nullptr;
  }

  void Drop(const std::string& logger_name) {}

  void DropAll() {}

 private:
  RegistryT<Mutex>() {}
  RegistryT<Mutex>(const RegistryT<Mutex>&) = delete;
  RegistryT<Mutex>& operator=(const RegistryT<Mutex>&) = delete;

  Mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
  FormatterPtr formatter_;
  level::level_enum level_ = level::info;

  bool async_mode_ = false;
  size_t async_q_size_ = 0;
  async_overflow_policy overflow_policy_ = async_overflow_policy::block_retry;
  std::function<void()> worker_warmup_cb_ = nullptr;
  std::function<void()> worker_teardown_cb_ = nullptr;
  std::chrono::milliseconds flush_interval_ms_;
};

#ifdef SPDLOG_NO_REGISTRY_MUTEX
using Registry = RegistryT<spdlog::details::null_mutex>;
#else
using Registry = RegistryT<std::mutex>;
#endif

}  // namespace spdlog

#endif  // SPDLOG_DETAILS_REGISTRY_H_