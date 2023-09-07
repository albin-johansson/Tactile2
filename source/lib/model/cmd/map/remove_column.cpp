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

#include "remove_column.hpp"

#include <utility>  // move

#include "common/util/functional.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "tactile/core/debug/error.hpp"

namespace tactile::cmd {

RemoveColumn::RemoveColumn(Shared<Map> map)
    : mMap {std::move(map)}
{
  if (!mMap) {
    throw Error {"Invalid null map"};
  }
}

void RemoveColumn::undo()
{
  invoke_n(mColumnCount, [this] { mMap->add_column(); });
  mCache.restore_tiles(*mMap);
}

void RemoveColumn::redo()
{
  const auto map_extent = mMap->get_extent();

  const auto begin = TilePos::from(0u, map_extent.cols - mColumnCount - 1u);
  const auto end = TilePos::from(map_extent.rows, map_extent.cols);

  mCache.clear();
  mCache.save_tiles(*mMap, begin, end);

  invoke_n(mColumnCount, [this] { mMap->remove_column(); });
}

auto RemoveColumn::merge_with(const Command* cmd) -> bool
{
  if (const auto* other = dynamic_cast<const RemoveColumn*>(cmd)) {
    mColumnCount += other->mColumnCount;
    mCache.merge_with(other->mCache);
    return true;
  }

  return false;
}

auto RemoveColumn::get_name() const -> String
{
  const auto& lang = get_current_language();
  return mColumnCount == 1 ? lang.cmd.remove_column : lang.cmd.remove_columns;
}

}  // namespace tactile::cmd