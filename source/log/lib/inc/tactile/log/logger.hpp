// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <chrono>       // steady_clock
#include <format>       // format_args, make_format_args
#include <memory>       // unique_ptr
#include <string_view>  // string_view

#include "tactile/base/prelude.hpp"
#include "tactile/base/util/format.hpp"
#include "tactile/log/log_level.hpp"
#include "tactile/log/log_sink.hpp"

namespace tactile::log {

/**
 * A simple sink-based logger implementation.
 */
class Logger final
{
 public:
  using clock_type = std::chrono::steady_clock;

  TACTILE_DELETE_COPY(Logger);
  TACTILE_DECLARE_MOVE(Logger);

  Logger();

  ~Logger() noexcept;

  /**
   * Logs an arbitrary message.
   *
   * \tparam Args The format argument types.
   *
   * \param level The severity of the message.
   * \param fmt   The format string.
   * \param args  The format arguments.
   */
  template <typename... Args>
  void log(const LogLevel level, const FormatString<Args...> fmt, const Args&... args) noexcept
  {
#if TACTILE_HAS_STD_FORMAT_STRING
    _log(level, fmt.get(), std::make_format_args(args...));
#else
    _log(level, fmt, std::make_format_args(args...));
#endif
  }

  /**
   * Adds a sink to the logger.
   *
   * \param sink A logger sink.
   */
  void add_sink(std::unique_ptr<ILogSink> sink);

  /**
   * Sets a reference time point to use as a relative baseline for timestamps.
   *
   * \param instant The new reference instant.
   */
  void set_reference_instant(clock_type::time_point instant);

  /**
   * Sets the minimum severity of messages that get logged.
   *
   * \param level The minimum log level.
   */
  void set_log_level(LogLevel level);

  /**
   * Sets the minimum severity of messages that will initiate flushes.
   *
   * \param level The flush log level.
   */
  void set_flush_level(LogLevel level);

  /**
   * Indicates whether a message with a given severity would be logged.
   *
   * \param level The log level to consider.
   *
   * \return
   * True if the message would be logged; false otherwise.
   */
  [[nodiscard]]
  auto would_log(LogLevel level) const noexcept -> bool;

  /**
   * Indicates whether a message with a given severity would trigger a flush.
   *
   * \param level The log level to consider.
   *
   * \return
   * True if the message would cause a flush; false otherwise.
   */
  [[nodiscard]]
  auto would_flush(LogLevel level) const noexcept -> bool;

 private:
  struct Data;
  std::unique_ptr<Data> m_data {};

  void _log(LogLevel level, std::string_view fmt, std::format_args args) noexcept;
};

}  // namespace tactile::log

#define TACTILE_LOG(LoggerPtr, Lvl, Fmt, ...)                                             \
  do {                                                                                    \
    auto* const tactile_log_macro_logger_ptr = (LoggerPtr);                               \
    if (tactile_log_macro_logger_ptr && tactile_log_macro_logger_ptr->would_log((Lvl))) { \
      tactile_log_macro_logger_ptr->log((Lvl), (Fmt) __VA_OPT__(, ) __VA_ARGS__);         \
    }                                                                                     \
  } while (false)

#define TACTILE_LOG_TRACE(LoggerPtr, Fmt, ...) \
  TACTILE_LOG(LoggerPtr, ::tactile::log::LogLevel::kTrace, Fmt, __VA_ARGS__)

#define TACTILE_LOG_DEBUG(LoggerPtr, Fmt, ...) \
  TACTILE_LOG(LoggerPtr, ::tactile::log::LogLevel::kDebug, Fmt, __VA_ARGS__)

#define TACTILE_LOG_INFO(LoggerPtr, Fmt, ...) \
  TACTILE_LOG(LoggerPtr, ::tactile::log::LogLevel::kInfo, Fmt, __VA_ARGS__)

#define TACTILE_LOG_WARN(LoggerPtr, Fmt, ...) \
  TACTILE_LOG(LoggerPtr, ::tactile::log::LogLevel::kWarn, Fmt, __VA_ARGS__)

#define TACTILE_LOG_ERROR(LoggerPtr, Fmt, ...) \
  TACTILE_LOG(LoggerPtr, ::tactile::log::LogLevel::kError, Fmt, __VA_ARGS__)
