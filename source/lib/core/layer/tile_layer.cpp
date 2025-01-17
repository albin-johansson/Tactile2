// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tile_layer.hpp"

#include <utility>  // cmp_less

#include "common/util/algorithm.hpp"
#include "common/util/functional.hpp"
#include "core/tile/tile_matrix.hpp"
#include "core/tile/tile_pos.hpp"
#include "tactile/base/container/queue.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/core/debug/exception.hpp"
#include "tactile/core/debug/generic_error.hpp"
#include "tactile/core/numeric/vec.hpp"

namespace tactile {

TileLayer::TileLayer()
    : TileLayer {TileExtent {5, 5}}
{
}

TileLayer::TileLayer(const TileExtent extent)
    : mTiles {make_tile_matrix(extent)}
{
  if (extent.rows == 0) {
    throw Exception {"Invalid row count"};
  }
  else if (extent.cols == 0) {
    throw Exception {"Invalid column count"};
  }
}

void TileLayer::accept(ContextVisitor& visitor) const
{
  visitor.visit(*this);
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
                      Vector<TilePos>* affected)
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
    row.push_back(kEmptyTile);
  }
}

auto TileLayer::remove_row() -> Result<void>
{
  if (!mTiles.empty()) {
    mTiles.pop_back();
    return kOK;
  }
  else {
    return unexpected(make_error(GenericError::kInvalidState));
  }
}

auto TileLayer::remove_column() -> Result<void>
{
  for (auto& row: mTiles) {
    if (!row.empty()) {
      row.pop_back();
    }
    else {
      return unexpected(make_error(GenericError::kInvalidState));
    }
  }

  return kOK;
}

void TileLayer::resize(const TileExtent extent)
{
  if (extent.rows == 0) {
    throw Exception {"Invalid row count"};
  }
  else if (extent.cols == 0) {
    throw Exception {"Invalid column count"};
  }

  const auto current_rows = row_count();
  const auto current_cols = column_count();

  if (const auto n = udiff(current_rows, extent.rows); current_rows < extent.rows) {
    invoke_n(n, [this] { add_row(); });
  }
  else {
    invoke_n(n, [this] { remove_row(); });
  }

  if (const auto n = udiff(current_cols, extent.cols); current_cols < extent.cols) {
    invoke_n(n, [this] { add_column(); });
  }
  else {
    invoke_n(n, [this] { remove_column(); });
  }
}

auto TileLayer::set_tile(const TilePos& pos, const TileID id) -> Result<void>
{
  if (is_valid(pos)) [[likely]] {
    mTiles[static_cast<usize>(pos.row())][static_cast<usize>(pos.col())] = id;
    return kOK;
  }
  else {
    return unexpected(make_error(GenericError::kInvalidParam));
  }
}

auto TileLayer::tile_at(const TilePos& pos) const -> Maybe<TileID>
{
  if (is_valid(pos)) [[likely]] {
    return mTiles[static_cast<usize>(pos.row())][static_cast<usize>(pos.col())];
  }
  else {
    return nothing;
  }
}

auto TileLayer::is_valid(const TilePos& pos) const -> bool
{
  return pos.row() >= 0 &&  //
         pos.col() >= 0 &&  //
         std::cmp_less(pos.row(), row_count()) &&
         std::cmp_less(pos.col(), column_count());
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
