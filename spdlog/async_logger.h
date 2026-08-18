//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_ASYNC_LOGGER_H_
#define SPDLOG_ASYNC_LOGGER_H_

#include <spdlog/logger.h>

namespace spdlog {

class AsyncLogger : public Logger {};

}  // namespace spdlog

#include <spdlog/details/async_logger_impl.h>

#endif  // SPDLOG_ASYNC_LOGGER_H_