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

#include "tile_layer.hpp"

#include "core/tile/tile_matrix.hpp"
#include "core/tile/tile_pos.hpp"
#include "core/type/math.hpp"
#include "core/type/queue.hpp"
#include "core/util/functional.hpp"
#include "core/util/numeric.hpp"
#include "core/debug/assert.hpp"
#include "core/debug/panic.hpp"

namespace tactile {

TileLayer::TileLayer()
    : TileLayer {5, 5}
{
}

TileLayer::TileLayer(const usize rows, const usize columns)
    : mTiles {make_tile_matrix(rows, columns)}
{
  if (rows == 0) {
    throw TactileError {"Invalid row count!"};
  }
  else if (columns == 0) {
    throw TactileError {"Invalid column count!"};
  }
}

void TileLayer::accept(LayerVisitor& visitor)
{
  visitor.visit(*this);
}

void TileLayer::accept(ConstLayerVisitor& visitor) const
{
  visitor.visit(*this);
}

void TileLayer::flood(const TilePos& origin,
                      const TileID replacement,
                      Vec<TilePos>* affected)
{
  const auto target = tile_at(origin);

  if (!is_valid(origin) || (target == replacement)) {
    return;
  }

  Queue<TilePos> positions;
  positions.push(origin);

  set_tile(origin, replacement);
  if (affected) {
    affected->push_back(origin);
  }

  /* Determines whether a position should be flooded */
  auto check = [&](const TilePos& position) {
    if (is_valid(position)) {
      const auto tile = tile_at(position);
      if (tile == target) {
        set_tile(position, replacement);
        if (affected) {
          affected->push_back(position);
        }
        positions.push(position);
      }
    }
  };

  while (!positions.empty()) {
    const auto position = positions.front();
    positions.pop();

    check(position.west());
    check(position.east());
    check(position.south());
    check(position.north());
  }
}

void TileLayer::add_row()
{
  mTiles.push_back(make_tile_row(column_count()));
}

void TileLayer::add_column()
{
  for (auto& row: mTiles) {
    row.push_back(empty_tile);
  }
}

void TileLayer::remove_row()
{
  TACTILE_ASSERT(row_count() > 1);
  mTiles.pop_back();
}

void TileLayer::remove_column()
{
  for (auto& row: mTiles) {
    TACTILE_ASSERT(row.size() > 1);
    row.pop_back();
  }
}

void TileLayer::resize(const usize rows, const usize columns)
{
  if (rows == 0) {
    throw TactileError {"Invalid row count!"};
  }
  else if (columns == 0) {
    throw TactileError {"Invalid column count!"};
  }

  const auto current_rows = row_count();
  const auto current_cols = column_count();

  if (const auto n = udiff(current_rows, rows); current_rows < rows) {
    invoke_n(n, [this] { add_row(); });
  }
  else {
    invoke_n(n, [this] { remove_row(); });
  }

  if (const auto n = udiff(current_cols, columns); current_cols < columns) {
    invoke_n(n, [this] { add_column(); });
  }
  else {
    invoke_n(n, [this] { remove_column(); });
  }
}

void TileLayer::accept(ContextVisitor& visitor) const
{
  visitor.visit(*this);
}

void TileLayer::set_tile(const TilePos& pos, const TileID id)
{
  if (is_valid(pos)) [[likely]] {
    mTiles[pos.urow()][pos.ucol()] = id;
  }
  else {
    throw TactileError {"Invalid position!"};
  }
}

auto TileLayer::tile_at(const TilePos& pos) const -> TileID
{
  if (is_valid(pos)) [[likely]] {
    return mTiles[pos.urow()][pos.ucol()];
  }
  else {
    throw TactileError {"Invalid position!"};
  }
}

auto TileLayer::is_valid(const TilePos& pos) const -> bool
{
  return pos.row() >= 0 &&            //
         pos.col() >= 0 &&            //
         pos.urow() < row_count() &&  //
         pos.ucol() < column_count();
}

auto TileLayer::row_count() const -> usize
{
  return mTiles.size();
}

auto TileLayer::column_count() const -> usize
{
  TACTILE_ASSERT(!mTiles.empty());
  return mTiles.at(0).size();
}

auto TileLayer::get_tiles() const -> const TileMatrix&
{
  return mTiles;
}

auto TileLayer::clone() const -> Shared<Layer>
{
  auto layer = std::make_shared<TileLayer>();
  layer->mDelegate = mDelegate.clone();
  layer->mTiles = mTiles;
  return layer;
}

}  // namespace tactile
