/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "tactile/core/common/assert.hpp"

#include <exception>  // terminate

#include <boost/stacktrace.hpp>
#include <spdlog/spdlog.h>

namespace tactile {

void on_assertion_failed(const char* expr,
                         const char* msg,
                         const char* file,
                         const int64 line)
{
  spdlog::critical("{}:{} expression '{}' evaluated to false: {}",
                   file,
                   line,
                   expr,
                   msg ? msg : "?");
  std::terminate();
}

}  // namespace tactile