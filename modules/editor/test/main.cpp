// Copyright (C) 2023 Albin Johansson (GNU General Public License v3.0)

#include <gtest/gtest.h>

#include "tactile/core/debug/log/logger_builder.hpp"
#include "tactile/core/misc/scope_guard.hpp"

using namespace tactile;

auto main(int argc, char* argv[]) -> int
{
  auto logger = LoggerBuilder {}
                    .use_initialization_time_as_reference_instant()
                    .min_level(LogLevel::kDebug)
                    .flush_on(LogLevel::kError)
                    .with_file_sink("editor_test_log.txt")
                    .with_terminal_sink()
                    .use_colored_terminal_output()
                    .build();

  set_default_logger(&logger);
  const ScopeGuard logger_guard {[] { set_default_logger(nullptr); }};

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}