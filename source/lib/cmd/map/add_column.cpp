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

#include "add_column.hpp"

#include "common/util/functional.hpp"
#include "components/map.hpp"
#include "model/context.hpp"
#include "model/systems/map_system.hpp"
#include "systems/language_system.hpp"

namespace tactile::cmd {

AddColumn::AddColumn(const Entity map_entity)
    : mMapEntity {map_entity}
{
}

void AddColumn::undo()
{
  auto& model = get_global_model();
  auto& map = model.get<Map>(mMapEntity);
  invoke_n(mColumnCount, [&] { sys::remove_column_from_map(model, map); });
}

void AddColumn::redo()
{
  auto& model = get_global_model();
  auto& map = model.get<Map>(mMapEntity);
  invoke_n(mColumnCount, [&] { sys::add_column_to_map(model, map); });
}

auto AddColumn::merge_with(const Command* cmd) -> bool
{
  if (const auto* other = dynamic_cast<const AddColumn*>(cmd)) {
    mColumnCount += other->mColumnCount;
    return true;
  }

  return false;
}

auto AddColumn::get_name() const -> String
{
  const auto& strings = sys::get_current_language_strings(get_global_model());
  return mColumnCount == 1 ? strings.cmd.add_column : strings.cmd.add_columns;
}

}  // namespace tactile::cmd
