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

#include <vector>  // vector

#include <boost/uuid/uuid_hash.hpp>

#include "core/common/assoc.hpp"
#include "core/common/fs.hpp"
#include "core/common/math.hpp"
#include "core/common/uuid.hpp"
#include "core/common/vocabulary.hpp"
#include "core/ctx/context.hpp"
#include "core/ctx/context_info.hpp"
#include "core/tile_pos.hpp"
#include "core/tileset/tile.hpp"
#include "core/tileset/tileset_info.hpp"
#include "core/type/ptr.hpp"

namespace tactile {

class Tileset final : public Context {
 public:
  Tileset(const UUID& id, TilesetInfo info);

  explicit Tileset(TilesetInfo info);

  void accept(ContextVisitor& visitor) const override;

  void update();

  [[nodiscard]] auto operator[](TileIndex index) -> Tile&;
  [[nodiscard]] auto operator[](TileIndex index) const -> const Tile&;

  [[nodiscard]] auto get_tile(TileIndex index) -> const Shared<Tile>&;

  [[nodiscard]] auto index_of(const TilePos& pos) const -> TileIndex;

  /// Returns the index of the tile that should be rendered for a specific tile.
  [[nodiscard]] auto appearance_of(TileIndex index) const -> TileIndex;

  [[nodiscard]] auto ctx() -> ContextInfo& override;
  [[nodiscard]] auto ctx() const -> const ContextInfo& override;

  [[nodiscard]] auto get_uuid() const -> const UUID& override;

  [[nodiscard]] auto texture_id() const noexcept -> uint { return mTextureId; }

  [[nodiscard]] auto texture_path() const -> const fs::path& { return mTexturePath; }

  [[nodiscard]] auto texture_size() const noexcept -> const Int2& { return mTextureSize; }

  [[nodiscard]] auto tile_size() const noexcept -> const Int2& { return mTileSize; }

  [[nodiscard]] auto uv_size() const noexcept -> const Float2& { return mUvSize; }

  [[nodiscard]] auto row_count() const noexcept -> int32 { return mRowCount; }

  [[nodiscard]] auto column_count() const noexcept -> int32 { return mColumnCount; }

  [[nodiscard]] auto tile_count() const noexcept -> int32
  {
    return row_count() * column_count();
  }

  [[nodiscard]] auto begin() const noexcept { return mMetaTiles.begin(); }

  [[nodiscard]] auto end() const noexcept { return mMetaTiles.end(); }

 private:
  ContextInfo mContext;
  uint mTextureId {};
  Int2 mTextureSize {};
  Int2 mTileSize {};
  int32 mRowCount {};
  int32 mColumnCount {};
  Float2 mUvSize {};
  HashMap<TileIndex, UUID> mIdentifiers;
  HashMap<UUID, Shared<Tile>> mMetaTiles;
  fs::path mTexturePath;

  /// A cache of the tiles that should be rendered when a tile is encountered
  mutable HashMap<TileIndex, TileIndex> mAppearanceCache;

  void load_tiles();

  [[nodiscard]] auto is_valid(TileIndex index) const -> bool;
};

}  // namespace tactile
