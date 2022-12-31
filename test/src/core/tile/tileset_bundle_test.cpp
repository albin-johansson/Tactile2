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

#include "core/tile/tileset_bundle.hpp"

#include <gtest/gtest.h>

#include "core/debug/panic.hpp"
#include "core/tile/tileset_info.hpp"
#include "io/load_texture.hpp"

namespace tactile::test {
namespace {

[[nodiscard]] auto make_tileset() -> Shared<Tileset>
{
  return std::make_shared<Tileset>(
      TilesetInfo {.texture = io::load_texture("resources/terrain.png"),
                   .tile_size = {32, 32}});
}

}  // namespace

TEST(TilesetBundle, Defaults)
{
  const TilesetBundle bundle;
  ASSERT_EQ(1, bundle.get_next_tile_id());
  ASSERT_EQ(0u, bundle.size());
  ASSERT_TRUE(bundle.empty());
  ASSERT_FALSE(bundle.get_active_tileset_id().has_value());
}

TEST(TilesetBundle, AttachTilesetWithExplicitFirstTileId)
{
  const TileID first_tile = 123;

  TilesetBundle bundle;

  auto tileset = make_tileset();
  bundle.attach_tileset(tileset, first_tile, false);

  ASSERT_EQ(1u, bundle.size());
  ASSERT_EQ(1, bundle.get_next_tile_id());
  ASSERT_FALSE(bundle.empty());

  ASSERT_TRUE(bundle.is_valid_tile(first_tile));
  ASSERT_TRUE(bundle.is_valid_tile(first_tile + 42));
  ASSERT_TRUE(bundle.is_valid_tile(first_tile + tileset->tile_count()));

  ASSERT_FALSE(bundle.is_valid_tile(first_tile - 1));
  ASSERT_FALSE(bundle.is_valid_tile(first_tile + tileset->tile_count() + 1));

  const auto& ref = bundle.get_tileset_ref(tileset->get_uuid());
  ASSERT_EQ(tileset->tile_count(), ref.get_last_tile() - ref.get_first_tile());
  ASSERT_EQ(first_tile, ref.get_first_tile());
  ASSERT_EQ(first_tile + tileset->tile_count(), ref.get_last_tile());
}

TEST(TilesetBundle, AttachTileset)
{
  TilesetBundle bundle;

  const auto first_tile = bundle.get_next_tile_id();

  auto tileset = make_tileset();
  bundle.attach_tileset(tileset, false);

  ASSERT_EQ(1u, bundle.size());
  ASSERT_EQ(first_tile + tileset->tile_count() + 1, bundle.get_next_tile_id());
  ASSERT_FALSE(bundle.empty());

  ASSERT_TRUE(bundle.is_valid_tile(first_tile));
  ASSERT_TRUE(bundle.is_valid_tile(first_tile + 39));
  ASSERT_TRUE(bundle.is_valid_tile(first_tile + tileset->tile_count()));

  ASSERT_FALSE(bundle.is_valid_tile(first_tile - 1));
  ASSERT_FALSE(bundle.is_valid_tile(first_tile + tileset->tile_count() + 1));

  const auto& ref = bundle.get_tileset_ref(tileset->get_uuid());
  ASSERT_EQ(tileset->tile_count(), ref.get_last_tile() - ref.get_first_tile());
  ASSERT_EQ(first_tile, ref.get_first_tile());
  ASSERT_EQ(first_tile + tileset->tile_count(), ref.get_last_tile());
}

TEST(TilesetBundle, DetachTileset)
{
  TilesetBundle bundle;

  auto tileset = make_tileset();
  const auto tileset_id = tileset->get_uuid();

  bundle.attach_tileset(tileset, false);

  ASSERT_EQ(1u, bundle.size());
  ASSERT_TRUE(bundle.has_tileset(tileset_id));

  ASSERT_TRUE(bundle.is_valid_tile(1));
  ASSERT_TRUE(bundle.is_valid_tile(42));

  bundle.detach_tileset(tileset_id);

  ASSERT_EQ(0u, bundle.size());
  ASSERT_FALSE(bundle.has_tileset(tileset_id));

  ASSERT_FALSE(bundle.is_valid_tile(1));
  ASSERT_FALSE(bundle.is_valid_tile(42));

  ASSERT_THROW(bundle.detach_tileset(tileset_id), TactileError);
}

TEST(TilesetBundle, SelectTileset)
{
  TilesetBundle bundle;

  const auto a = make_tileset();
  const auto b = make_tileset();

  bundle.attach_tileset(a, false);
  bundle.attach_tileset(b, false);

  ASSERT_FALSE(bundle.get_active_tileset_id().has_value());

  bundle.select_tileset(a->get_uuid());
  ASSERT_EQ(a->get_uuid(), bundle.get_active_tileset_id());

  bundle.select_tileset(b->get_uuid());
  ASSERT_EQ(b->get_uuid(), bundle.get_active_tileset_id());

  bundle.detach_tileset(b->get_uuid());
  ASSERT_EQ(a->get_uuid(), bundle.get_active_tileset_id());

  bundle.detach_tileset(a->get_uuid());
  ASSERT_FALSE(bundle.get_active_tileset_id());
}

TEST(TilesetBundle, HasTileset)
{
  TilesetBundle bundle;

  const auto tileset = make_tileset();
  ASSERT_FALSE(bundle.has_tileset(tileset->get_uuid()));

  bundle.attach_tileset(tileset, true);
  ASSERT_TRUE(bundle.has_tileset(tileset->get_uuid()));
}

TEST(TilesetBundle, GetRef)
{
  TilesetBundle bundle;

  const auto a = make_tileset();
  const auto b = make_tileset();

  bundle.attach_tileset(a, false);
  bundle.attach_tileset(b, true);

  const auto& aa = bundle.get_tileset_ref(a->get_uuid());
  const auto& bb = bundle.get_tileset_ref(b->get_uuid());

  ASSERT_EQ(1, aa.get_first_tile());
  ASSERT_EQ(aa.get_last_tile() + 1, bb.get_first_tile());

  ASSERT_EQ(a->get_uuid(), aa.get_tileset().get_uuid());
  ASSERT_EQ(b->get_uuid(), bb.get_tileset().get_uuid());

  ASSERT_FALSE(aa.get_selection().has_value());
  ASSERT_FALSE(bb.get_selection().has_value());

  ASSERT_FALSE(aa.is_embedded());
  ASSERT_TRUE(bb.is_embedded());
}

TEST(TilesetBundle, ToLocalIndex)
{
  TilesetBundle bundle;

  const auto a = make_tileset();
  const auto b = make_tileset();

  bundle.attach_tileset(a, false);
  bundle.attach_tileset(b, false);

  auto& ref_a = bundle.get_tileset_ref(a->get_uuid());
  auto& ref_b = bundle.get_tileset_ref(b->get_uuid());

  ASSERT_EQ(1, ref_a.get_first_tile());
  ASSERT_EQ(1 + a->tile_count(), ref_a.get_last_tile());

  ASSERT_EQ(ref_a.get_last_tile() + 1, ref_b.get_first_tile());
  ASSERT_EQ(ref_b.get_first_tile() + b->tile_count(), ref_b.get_last_tile());

  ASSERT_EQ(0, bundle.to_tile_index(ref_a.get_first_tile()));
  ASSERT_EQ(a->tile_count(), bundle.to_tile_index(ref_a.get_last_tile()));

  ASSERT_EQ(0, bundle.to_tile_index(ref_b.get_first_tile()));
  ASSERT_EQ(b->tile_count(), bundle.to_tile_index(ref_b.get_last_tile()));

  ASSERT_THROW(bundle.to_tile_index(ref_a.get_first_tile() - 1), TactileError);
  ASSERT_THROW(bundle.to_tile_index(ref_b.get_last_tile() + 1), TactileError);
}

}  // namespace tactile::test
