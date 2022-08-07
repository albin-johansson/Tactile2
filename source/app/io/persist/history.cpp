/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "history.hpp"

#include <algorithm>  // find
#include <fstream>    // ifstream, ofstream
#include <ios>        // ios
#include <utility>    // move

#include <spdlog/spdlog.h>

#include "core/common/filesystem.hpp"
#include "core/common/ints.hpp"
#include "core/common/maybe.hpp"
#include "io/directories.hpp"
#include "io/persist/proto.hpp"
#include "misc/panic.hpp"

namespace tactile::io {
namespace {

constexpr int   history_format_version [[maybe_unused]] = 1;
constexpr usize history_max_size = 10;

/* We store paths as strings because that makes displaying them in menus
   _much_ easier (and faster) */
inline Maybe<std::string>      history_last_closed_file;
inline std::deque<std::string> history_entries;

[[nodiscard]] auto get_file_path() -> const fs::path&
{
  static const auto path = persistent_file_dir() / "history.bin";
  return path;
}

}  // namespace

void load_file_history()
{
  spdlog::debug("Loading file history...");
  std::ifstream stream {get_file_path(), std::ios::in | std::ios::binary};

  proto::History h;
  if (h.ParseFromIstream(&stream)) {
    if (h.has_last_opened_file()) {
      history_last_closed_file = h.last_opened_file();
    }

    for (auto file : h.files()) {
      if (fs::exists(file)) {
        spdlog::debug("Loaded '{}' from file history", file);
        history_entries.push_back(std::move(file));
      }
    }
  }
  else {
    spdlog::warn("Failed to read history file (this is expected for first time runs)");
  }
}

void save_file_history()
{
  proto::History h;

  if (history_last_closed_file) {
    h.set_last_opened_file(*history_last_closed_file);
  }

  for (const auto& path : history_entries) {
    spdlog::debug("Saving '{}' to file history", path);
    h.add_files(path);
  }

  std::ofstream stream {get_file_path(),
                        std::ios::out | std::ios::trunc | std::ios::binary};
  if (!h.SerializeToOstream(&stream)) {
    spdlog::error("Failed to save file history!");
  }
}

void clear_file_history()
{
  spdlog::debug("Clearing file history...");
  history_entries.clear();
}

void add_file_to_history(const fs::path& path)
{
  auto converted = convert_to_forward_slashes(path);
  if (std::find(history_entries.begin(), history_entries.end(), converted) ==
      history_entries.end()) {
    spdlog::debug("Adding '{}' to history...", converted);
    history_entries.push_back(std::move(converted));

    if (history_entries.size() > history_max_size) {
      history_entries.pop_front();
    }
  }
  else {
    spdlog::debug("Did not add existing entry '{}' to file history", converted);
  }
}

void set_last_closed_file(const fs::path& path)
{
  history_last_closed_file = convert_to_forward_slashes(path);
  spdlog::trace("Last closed file is now '{}'", *history_last_closed_file);

  add_file_to_history(path);
}

auto file_history() -> const std::deque<std::string>&
{
  return history_entries;
}

auto is_last_closed_file_valid() -> bool
{
  return history_last_closed_file && fs::exists(*history_last_closed_file);
}

auto last_closed_file() -> const std::string&
{
  if (is_last_closed_file_valid()) {
    return history_last_closed_file.value();
  }
  else {
    throw TactileError {"Invalid last closed file!"};
  }
}

}  // namespace tactile::io
