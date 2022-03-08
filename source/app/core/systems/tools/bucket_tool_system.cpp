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

#include "bucket_tool_system.hpp"

#include "core/components/tileset.hpp"
#include "core/systems/layers/layer_system.hpp"
#include "core/systems/tileset_system.hpp"
#include "editor/events/tool_events.hpp"
#include "misc/assert.hpp"

namespace tactile::sys {

void bucket_tool_on_pressed(entt::registry& registry,
                            entt::dispatcher& dispatcher,
                            const mouse_info& mouse)
{
  if (is_tile_layer_active(registry) && is_single_tile_selected_in_tileset(registry) &&
      mouse.button == cen::mouse_button::left) {
    const auto entity = find_active_tileset(registry);
    TACTILE_ASSERT(entity != entt::null);

    const auto& selection = registry.get<comp::tileset_selection>(entity);
    const auto position = selection.region->begin;
    const auto replacement = get_tile_from_tileset(registry, entity, position);

    dispatcher.enqueue<flood_event>(mouse.position_in_viewport, replacement);
  }
}

}  // namespace tactile::sys
