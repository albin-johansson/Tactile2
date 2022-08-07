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

#include "core/common/fs.hpp"
#include "io/map/ir/ir.hpp"
#include "io/map/parse/parse_error.hpp"

namespace tactile::io {

class ParseResult final
{
 public:
  void set_path(const fs::path& path);

  void set_error(ParseError error);

  [[nodiscard]] auto path() const -> const fs::path&;

  [[nodiscard]] auto error() const -> ParseError;

  [[nodiscard]] auto data() -> ir::MapData&;
  [[nodiscard]] auto data() const -> const ir::MapData&;

 private:
  fs::path    mPath;
  ir::MapData mData;
  ParseError  mError {ParseError::None};
};

}  // namespace tactile::io