// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <filesystem>  // path
#include <fstream>     // ofstream

#include "tactile/log/log_sink.hpp"

namespace tactile::log {

/**
 * A log sink that writes messages to a text file.
 */
class FileLogSink final : public ILogSink
{
 public:
  explicit FileLogSink(const std::filesystem::path& path);

  void log(const LogMessage& msg) override;

  void flush() override;

 private:
  std::ofstream m_stream;
};

}  // namespace tactile::log
