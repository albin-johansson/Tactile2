// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/log/logger.hpp"

#include <cstdio>     // fprintf, stderr
#include <exception>  // exception
#include <optional>   // optional
#include <utility>    // move
#include <vector>     // vector

#include "tactile/base/container/buffer.hpp"
#include "tactile/base/util/format.hpp"
#include "tactile/log/log_sink.hpp"

namespace tactile::log {

struct Logger::Data final
{
  LogLevel log_level {LogLevel::kInfo};
  LogLevel flush_level {LogLevel::kError};
  std::vector<std::unique_ptr<ILogSink>> sinks {};
  std::optional<clock_type::time_point> ref_instant {};
  Buffer<char, 1024> text_buffer {};
  Buffer<char, 20> prefix_buffer {};
};

Logger::Logger()
  : m_data {std::make_unique<Data>()}
{}

Logger::~Logger() noexcept = default;

TACTILE_DEFINE_MOVE(Logger);

void Logger::_log(const LogLevel level,
                  const std::string_view fmt,
                  std::format_args args) noexcept
{
  try {
    auto& data = *m_data;

    if (data.sinks.empty()) {
      return;
    }

    const auto log_instant = clock_type::now();
    const auto elapsed_time =
        log_instant - data.ref_instant.value_or(clock_type::time_point {});

    data.text_buffer.clear();
    data.prefix_buffer.clear();

    vformat_to_buffer(data.text_buffer, fmt, std::move(args));
    format_to_buffer(data.prefix_buffer, "[{} {:.>12%Q}]", "XXX", elapsed_time);

    const LogMessage message {
      .level = level,
      .prefix = data.prefix_buffer.view(),
      .text = data.text_buffer.view(),
    };

    const auto do_flush = would_flush(level);

    for (const auto& sink : data.sinks) {
      sink->log(message);

      if (do_flush) {
        sink->flush();
      }
    }
  }
  catch (const std::exception& error) {
    std::fprintf(stderr, "Logger error: %s", error.what());
  }
  catch (...) {
    std::fprintf(stderr, "Logger error");
  }
}

void Logger::add_sink(std::unique_ptr<ILogSink> sink)
{
  if (sink) {
    m_data->sinks.push_back(std::move(sink));
  }
}

void Logger::set_reference_instant(const clock_type::time_point instant)
{
  m_data->ref_instant = instant;
}

void Logger::set_log_level(const LogLevel level)
{
  m_data->log_level = level;
}

void Logger::set_flush_level(const LogLevel level)
{
  m_data->flush_level = level;
}

auto Logger::would_log(const LogLevel level) const noexcept -> bool
{
  return level >= m_data->log_level;
}

auto Logger::would_flush(const LogLevel level) const noexcept -> bool
{
  return level >= m_data->flush_level;
}

}  // namespace tactile::log
