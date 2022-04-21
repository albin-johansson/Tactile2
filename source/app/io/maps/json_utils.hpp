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

#include <filesystem>   // path
#include <optional>     // optional
#include <string>       // string
#include <string_view>  // string_view

#include <nlohmann/json.hpp>

#include "core/attribute.hpp"
#include "tactile.hpp"

namespace tactile {

NLOHMANN_JSON_SERIALIZE_ENUM(AttributeType,
                             {
                                 {AttributeType::String, "string"},
                                 {AttributeType::Int, "int"},
                                 {AttributeType::Float, "float"},
                                 {AttributeType::Bool, "bool"},
                                 {AttributeType::Object, "object"},
                                 {AttributeType::Color, "color"},
                                 {AttributeType::Path, "file"},
                             })

void to_json(nlohmann::json& json, const Attribute& value);

void write_json(const nlohmann::json& json, const std::filesystem::path& path);

[[nodiscard]] auto read_json(const std::filesystem::path& path)
    -> std::optional<nlohmann::json>;

[[nodiscard]] auto as_string(const nlohmann::json& json, std::string_view name)
    -> std::optional<std::string>;

[[nodiscard]] auto as_int(const nlohmann::json& json, std::string_view name)
    -> std::optional<int32>;

[[nodiscard]] auto as_uint(const nlohmann::json& json, std::string_view name)
    -> std::optional<uint32>;

[[nodiscard]] auto as_float(const nlohmann::json& json, std::string_view name)
    -> std::optional<float>;

[[nodiscard]] auto as_bool(const nlohmann::json& json, std::string_view name)
    -> std::optional<bool>;

}  // namespace tactile