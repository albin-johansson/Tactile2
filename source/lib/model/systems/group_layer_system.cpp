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

#include "group_layer_system.hpp"

#include <algorithm>  // find, iter_swap

#include "common/debug/assert.hpp"
#include "common/type/maybe.hpp"
#include "common/util/algorithm.hpp"
#include "common/util/functional.hpp"
#include "core/map.hpp"
#include "model/systems/layer_system.hpp"

namespace tactile::sys {
namespace {

[[nodiscard]] auto _get_sibling_count(const Model& model,
                                      const CGroupLayer& root,
                                      const Entity target_layer_entity) -> Maybe<usize>
{
  for (const auto child_layer_entity: root.children) {
    if (child_layer_entity == target_layer_entity) {
      return root.children.size() - 1;
    }

    if (const auto* child_group = model.try_get<CGroupLayer>(child_layer_entity)) {
      if (const auto count =
              _get_sibling_count(model, *child_group, target_layer_entity)) {
        return *count;
      }
    }
  }

  return nothing;
}

void _offset_layer(Model& model,
                   const Entity root_layer_entity,
                   const Entity layer_entity,
                   const ssize offset)
{
  TACTILE_ASSERT(is_layer_entity(model, layer_entity));

  const auto parent_layer_entity =
      get_parent_layer(model, root_layer_entity, layer_entity);
  auto& parent_layer = model.get<CGroupLayer>(parent_layer_entity);

  const auto begin = parent_layer.children.begin();
  const auto end = parent_layer.children.end();

  if (const auto iter = std::find(begin, end, layer_entity); iter != end) {
    std::iter_swap(iter, iter + offset);
  }
}

}  // namespace

void move_layer_up(Model& model,
                   const Entity root_layer_entity,
                   const Entity layer_entity)
{
  TACTILE_ASSERT(can_move_layer_up(model,  //
                                   model.get<CGroupLayer>(root_layer_entity),
                                   layer_entity));
  _offset_layer(model, root_layer_entity, layer_entity, -1);
}

void move_layer_down(Model& model,
                     const Entity root_layer_entity,
                     const Entity layer_entity)
{
  TACTILE_ASSERT(can_move_layer_down(model,
                                     model.get<CGroupLayer>(root_layer_entity),
                                     layer_entity));
  _offset_layer(model, root_layer_entity, layer_entity, 1);
}

void set_layer_local_index(Model& model,
                           const Entity root_layer_entity,
                           const Entity layer_entity,
                           const usize new_index)
{
  TACTILE_ASSERT(is_group_layer_entity(model, root_layer_entity));
  TACTILE_ASSERT(is_layer_entity(model, layer_entity));

  const auto& root = model.get<CGroupLayer>(root_layer_entity);

  const auto current_local_index = get_local_layer_index(model, root, layer_entity);
  const auto steps = udiff(current_local_index.value(), new_index);

  if (new_index < current_local_index) {
    invoke_n(steps, [&] { move_layer_up(model, root_layer_entity, layer_entity); });
  }
  else {
    invoke_n(steps, [&] { move_layer_down(model, root_layer_entity, layer_entity); });
  }
}

auto can_move_layer_up(const Model& model,
                       const CGroupLayer& root,
                       const Entity layer_entity) -> bool
{
  return get_local_layer_index(model, root, layer_entity) != 0;
}

auto can_move_layer_down(const Model& model,
                         const CGroupLayer& root,
                         const Entity layer_entity) -> bool
{
  const auto local_index = get_local_layer_index(model, root, layer_entity);
  const auto sibling_count = _get_sibling_count(model, root, layer_entity);
  return local_index < sibling_count;
}

auto get_local_layer_index(const Model& model,
                           const CGroupLayer& root,
                           const Entity layer_entity) -> Maybe<usize>
{
  TACTILE_ASSERT(is_layer_entity(model, layer_entity));

  usize local_index = 0;
  for (const auto child_layer_entity: root.children) {
    if (child_layer_entity == layer_entity) {
      return local_index;
    }

    if (const auto* child_group = model.try_get<CGroupLayer>(child_layer_entity)) {
      if (const auto recursive_local_index =
              get_local_layer_index(model, *child_group, layer_entity)) {
        return *recursive_local_index;
      }
    }

    ++local_index;
  }

  return nothing;
}

}  // namespace tactile::sys