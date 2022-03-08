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

#pragma once

#include <filesystem>  // path
#include <string>      // string

#include <centurion.hpp>
#include <imgui.h>

#include "core/attribute.hpp"
#include "tactile.hpp"

namespace tactile {

[[nodiscard]] auto input_attribute(const char* id, const Attribute& value)
    -> maybe<Attribute>;

[[nodiscard]] auto input_int(const char* id, int value) -> maybe<int>;

[[nodiscard]] auto input_float(const char* id, float value, float min = 0, float max = 0)
    -> maybe<float>;

[[nodiscard]] auto input_string_with_hint(const char* id,
                                          const char* hint,
                                          const std::string& value,
                                          const char* label = nullptr,
                                          ImGuiInputTextFlags flags = 0,
                                          ImGuiInputTextCallback filter = nullptr)
    -> maybe<std::string>;

[[nodiscard]] auto input_string(const char* id,
                                const std::string& value,
                                const char* label = nullptr,
                                ImGuiInputTextFlags flags = 0,
                                ImGuiInputTextCallback filter = nullptr)
    -> maybe<std::string>;

[[nodiscard]] auto input_bool(const char* id, bool value) -> maybe<bool>;

[[nodiscard]] auto input_object(const char* id, object_t value) -> maybe<object_t>;

[[nodiscard]] auto input_color(const char* id, cen::color value) -> maybe<cen::color>;

[[nodiscard]] auto input_file(const char* id, const std::filesystem::path& value)
    -> maybe<std::filesystem::path>;

}  // namespace tactile
