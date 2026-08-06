//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_NULL_MUTEX_H_
#define SPDLOG_DETAILS_NULL_MUTEX_H_

namespace spdlog {
namespace details {

// null, no cost dummy "mutex" and dummy "atomic" int
struct null_mutex {
  void lock() {}
  void unlock() {}
  bool try_lock() { return true; }
};

struct null_atomic_int {
  int value;
  null_atomic_int() = default;
  null_atomic_int(int val) : value(val) {}
  int load(std::memory_order) const { return value; }
  void store(int val) { value = val; }
};

}  // namespace details
}  // namespace spdlog

#endif  // SPDLOG_DETAILS_NULL_MUTEX_H_