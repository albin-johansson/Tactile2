// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/io/ini.hpp"

#include <algorithm>  // all_of
#include <cctype>     // isspace
#include <fstream>    // ifstream
#include <string>     // getline
#include <utility>    // move

#include "tactile/base/container/string.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {
namespace {

inline constexpr char kCommentToken = ';';
inline constexpr char kAssignmentToken = '=';
inline constexpr char kSectionOpenToken = '[';
inline constexpr char kSectionCloseToken = ']';

[[nodiscard]]
auto _parse_section_header_name(const std::string_view current_line, const int line_number)
    -> std::expected<std::string, ErrorCode>
{
  TACTILE_ASSERT(current_line.starts_with(kSectionOpenToken));

  if (!current_line.ends_with(kSectionCloseToken)) {
    TACTILE_CORE_ERROR("Detected unterminated section (line {})", line_number);
    return std::unexpected {ErrorCode::kParseError};
  }

  TACTILE_ASSERT(current_line.size() >= 2);
  return std::string {current_line.substr(1, current_line.size() - 2)};
}

[[nodiscard]]
auto _parse_key_value_pair(const std::string_view current_line,
                           const int line_number,
                           IniSection& section) -> std::expected<void, ErrorCode>
{
  const auto eq_pos = current_line.find_first_of(kAssignmentToken);

  if (eq_pos == std::string::npos || eq_pos == 0) {
    TACTILE_CORE_ERROR("Detected missing assignment ('{}') operator (line {})",
                       kAssignmentToken,
                       line_number);
    return std::unexpected {ErrorCode::kParseError};
  }

  auto key = trim_string(current_line.substr(0, eq_pos));
  if (key.empty()) {
    TACTILE_CORE_ERROR("Detected empty key (line {})", line_number);
    return std::unexpected {ErrorCode::kParseError};
  }

  auto value = trim_string(current_line.substr(eq_pos + 1));

  const auto value_is_just_whitespace = std::ranges::all_of(value, [](const char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
  });

  if (value.empty() || value_is_just_whitespace) {
    TACTILE_CORE_ERROR("Detected empty value (line {})", line_number);
    return std::unexpected {ErrorCode::kParseError};
  }

  section.insert_or_assign(std::move(key), std::move(value));
  return {};
}

}  // namespace

auto parse_ini(const std::filesystem::path& path) -> std::expected<IniData, ErrorCode>
{
  TACTILE_CORE_DEBUG("Parsing INI from {}", path.string());

  IniData ini_data {};

  std::ifstream stream {path, std::ios::in};
  if (!stream.good()) {
    TACTILE_CORE_ERROR("Could not read file");
    return std::unexpected {ErrorCode::kBadFileStream};
  }

  int line_number {0};
  std::string current_section {};
  std::string current_line {};

  current_line.reserve(64);
  while (std::getline(stream, current_line)) {
    ++line_number;

    // Ignore empty lines and those with comments.
    if (current_line.empty() || current_line.starts_with(kCommentToken)) {
      continue;
    }

    if (current_line.starts_with(kSectionOpenToken)) {
      if (auto section_name = _parse_section_header_name(current_line, line_number)) {
        current_section = std::move(*section_name);

        // This is needed to include empty sections.
        ini_data[current_section];
      }
      else {
        return std::unexpected {section_name.error()};
      }
    }
    else {
      if (current_section.empty()) {
        TACTILE_CORE_ERROR("Detected entry without parent section (line {})", line_number);
        return std::unexpected {ErrorCode::kParseError};
      }

      const auto result =
          _parse_key_value_pair(current_line, line_number, ini_data[current_section]);
      if (!result) {
        return std::unexpected {result.error()};
      }
    }

    current_line.clear();
  }

  return ini_data;
}

}  // namespace tactile::core
