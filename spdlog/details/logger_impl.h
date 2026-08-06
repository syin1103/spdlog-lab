//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SPDLOG_DETAILS_LOGGER_IMPL_H_
#define SPDLOG_DETAILS_LOGGER_IMPL_H_

#include <spdlog/logger.h>

inline spdlog::Logger::Logger(const std::string& name,
                              spdlog::SinkPtr single_sink) {}

inline spdlog::Logger::Logger(const std::string& name,
                              SinksInitList sinks_list) {}

template <class It>
inline spdlog::Logger::Logger(const std::string& name, const It& begin,
                              const It& end) {}

inline spdlog::Logger::~Logger() = default;

#endif  // SPDLOG_DETAILS_LOGGER_IMPL_H_