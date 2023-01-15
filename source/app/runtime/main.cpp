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

#include <cstdlib>    // EXIT_SUCCESS, EXIT_FAILURE
#include <exception>  // exception

#include <centurion/message_box.hpp>
#include <fmt/format.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>

#include "common/debug/panic.hpp"
#include "common/util/fmt.hpp"
#include "init/app_initializer.hpp"
#include "runtime/app.hpp"

namespace {

void show_crash_message_box(const char* error_msg)
{
  cen::message_box::show(
      "Tactile crashed :(",
      fmt::format("Error message: {}\nPlease consider submitting a bug "
                  "report with reproduction steps at "
                  "https://github.com/albin-johansson/tactile",
                  error_msg),
      cen::message_box_type::error);
}

}  // namespace

auto main(int, char*[]) -> int
{
  try {
    tactile::AppInitializer initializer;

    tactile::App app {initializer.get_window()};
    app.start();

    return EXIT_SUCCESS;
  }
  catch (const tactile::TactileError& e) {
    show_crash_message_box(e.what());
    spdlog::critical("Unhandled exception message: '{}'\n{}", e.what(), e.get_trace());
    return EXIT_FAILURE;
  }
  catch (const std::exception& e) {
    show_crash_message_box(e.what());
    spdlog::critical("Unhandled exception message: '{}'", e.what());
    return EXIT_FAILURE;
  }
  catch (...) {
    show_crash_message_box("N/A");
    spdlog::critical("Unhandled value exception!");
    return EXIT_FAILURE;
  }
}