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

#include "io/maps/ir.hpp"
#include "parse_error.hpp"
#include "tactile.hpp"

namespace tactile::parsing {

class parse_data final
{
 public:
  void set_path(const std::filesystem::path& path);

  void set_error(parse_error error);

  [[nodiscard]] auto path() const -> const std::filesystem::path&;

  [[nodiscard]] auto error() const -> parse_error;

  [[nodiscard]] auto data() -> ir::map_data&;

  [[nodiscard]] auto data() const -> const ir::map_data&;

 private:
  std::filesystem::path mPath;
  ir::map_data mData;
  parse_error mError{parse_error::none};
};

}  // namespace tactile::parsing