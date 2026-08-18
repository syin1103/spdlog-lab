//
// Copyright(c) 2026 syin1103.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "logger.h"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "async_logger.h"

TEST(LoggerTest, SimpleTest) { fmt::print("Hello {}\n", "world"); }