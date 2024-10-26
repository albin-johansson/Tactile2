// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/log/terminal_log_sink.hpp"

#include <iostream>  // cout

namespace tactile::log {
namespace {

constexpr std::string_view kAnsiColorReset = "\x1B[0m";
constexpr std::string_view kAnsiColorFgRedBold = "\x1B[31m\x1B[1m";
constexpr std::string_view kAnsiColorFgYellow = "\x1B[33m";
constexpr std::string_view kAnsiColorFgMagenta = "\x1B[35m";
constexpr std::string_view kAnsiColorFgCyan = "\x1B[36m";
constexpr std::string_view kAnsiColorFgWhite = "\x1B[37m";

[[nodiscard]]
auto _get_text_ansi_color(const LogLevel level) noexcept -> std::string_view
{
  switch (level) {
    case LogLevel::kTrace: return kAnsiColorFgMagenta;
    case LogLevel::kDebug: return kAnsiColorFgCyan;
    case LogLevel::kInfo:  return kAnsiColorFgWhite;
    case LogLevel::kWarn:  return kAnsiColorFgYellow;
    case LogLevel::kError: return kAnsiColorFgRedBold;
  }

  return kAnsiColorFgWhite;
}

}  // namespace

void TerminalLogSink::log(const LogMessage& msg)
{
  if (m_use_ansi_colors) {
    const auto fg_color = _get_text_ansi_color(msg.level);
    std::cout << fg_color;
  }

  std::cout << msg.prefix << ' ' << msg.text;

  if (m_use_ansi_colors) {
    std::cout << kAnsiColorReset;
  }

  std::cout << '\n';
}

void TerminalLogSink::flush()
{
  std::cout.flush();
}

void TerminalLogSink::set_use_ansi_colors(const bool use_ansi_colors) noexcept
{
  m_use_ansi_colors = use_ansi_colors;
}

}  // namespace tactile::log
