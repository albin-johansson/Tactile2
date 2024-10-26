// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/log/log_sink.hpp"

namespace tactile::log {

/**
 * A log sink that simply forwards messages to the terminal.
 */
class TerminalLogSink final : public ILogSink
{
 public:
  void log(const LogMessage& msg) override;

  void flush() override;

  void set_use_ansi_colors(bool use_ansi_colors) noexcept;

 private:
  bool m_use_ansi_colors {};
};

}  // namespace tactile::log
