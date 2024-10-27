// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include <memory>   // make_unique
#include <utility>  // move

#include <gtest/gtest.h>

#include "tactile/base/util/scope_exit.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/platform/win32.hpp"
#include "tactile/log/terminal_log_sink.hpp"

auto main(int argc, char* argv[]) -> int
{
  tactile::core::win32_enable_virtual_terminal_processing();

  auto terminal_sink = std::make_unique<tactile::log::TerminalLogSink>();
  terminal_sink->set_use_ansi_colors(true);

  tactile::log::Logger logger {};
  logger.add_sink(std::move(terminal_sink));
  logger.set_log_level(tactile::log::LogLevel::kTrace);
  logger.set_reference_instant(tactile::log::Logger::clock_type::now());

  const tactile::ScopeExit logger_guard {[] { tactile::core::set_logger(nullptr); }};
  tactile::core::set_logger(&logger);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
