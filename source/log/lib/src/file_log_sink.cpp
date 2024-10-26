// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/log/file_log_sink.hpp"

#include <stdexcept>  // runtime_error

namespace tactile::log {

FileLogSink::FileLogSink(const std::filesystem::path& path)
  : m_stream {path, std::ios::out | std::ios::trunc}
{
  if (!m_stream.good()) {
    throw std::runtime_error {"bad file log sink"};
  }
}

void FileLogSink::log(const LogMessage& msg)
{
  m_stream << msg.prefix << ' ' << msg.text << '\n';
}

void FileLogSink::flush()
{
  m_stream.flush();
}

}  // namespace tactile::log
