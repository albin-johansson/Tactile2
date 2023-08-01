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

#include "godot_tileset.hpp"

#include <utility>  // move

#include <fmt/format.h>

#include "common/util/filesystem.hpp"
#include "core/debug/panic.hpp"

namespace tactile {

auto GodotTileset::add_texture(const Path& dest, Path source) -> GdExtRes
{
  const auto id =
      add_ext_resource(fmt::format("res://{}", to_forward_slashes_path(dest)), "Texture");
  mSourceTexturePaths.emplace_back(std::move(source), dest.filename().string());
  return id;
}

void GodotTileset::add_tileset(GdTilesetInfo info)
{
  mTilesetInfos.push_back(std::move(info));
}

auto GodotTileset::index_of(const UUID& tileset_id) const -> int32
{
  int32 index {};

  for (const auto& info: mTilesetInfos) {
    if (info.uuid == tileset_id) {
      return index;
    }

    ++index;
  }

  throw TactileError {"Invalid tileset identifier"};
}

auto GodotTileset::texture_paths() const -> const Vector<TextureNamePair>&
{
  return mSourceTexturePaths;
}

auto GodotTileset::tilesets() const -> const Vector<GdTilesetInfo>&
{
  return mTilesetInfos;
}

}  // namespace tactile