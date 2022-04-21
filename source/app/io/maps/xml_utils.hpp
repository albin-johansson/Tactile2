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

#include <optional>  // optional
#include <vector>    // vector

#include <pugixml.hpp>

#include "tactile.hpp"

namespace tactile {

[[nodiscard]] auto collect_children(pugi::xml_node source, const char* name)
    -> std::vector<pugi::xml_node>;

[[nodiscard]] auto has_attribute(pugi::xml_node node, const char* attributeName) -> bool;

[[nodiscard]] auto string_attribute(pugi::xml_node node, const char* attributeName)
    -> std::optional<std::string>;

[[nodiscard]] auto int_attribute(pugi::xml_node node, const char* attributeName)
    -> std::optional<int32>;

[[nodiscard]] auto uint_attribute(pugi::xml_node node, const char* attributeName)
    -> std::optional<uint32>;

[[nodiscard]] auto float_attribute(pugi::xml_node node, const char* attributeName)
    -> std::optional<float>;

[[nodiscard]] auto bool_attribute(pugi::xml_node node, const char* attributeName)
    -> std::optional<bool>;

}  // namespace tactile
