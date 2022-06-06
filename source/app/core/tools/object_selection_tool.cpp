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

#include "object_selection_tool.hpp"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

#include "core/common/ecs.hpp"
#include "core/components/attributes.hpp"
#include "core/components/objects.hpp"
#include "core/components/tools.hpp"
#include "core/events/object_events.hpp"
#include "core/events/tool_events.hpp"
#include "core/model.hpp"
#include "core/systems/layers/layer_system.hpp"
#include "core/systems/layers/object_layer_system.hpp"
#include "core/systems/viewport_system.hpp"

namespace tactile {

void ObjectSelectionTool::on_exited(DocumentModel& model, entt::dispatcher& dispatcher)
{
  maybe_emit_event(model, dispatcher);
}

void ObjectSelectionTool::on_pressed(DocumentModel& model,
                                     entt::dispatcher& dispatcher,
                                     const MouseInfo& mouse)
{
  auto& registry = model.get_active_registry();
  if (sys::is_object_layer_active(registry)) {
    auto& active = ctx_get<comp::ActiveState>(registry);
    auto& layer = checked_get<comp::ObjectLayer>(registry, active.layer);

    const auto objectEntity = sys::find_object(registry, layer, mouse.pos);

    switch (mouse.button) {
      case cen::mouse_button::left: {
        active.object = objectEntity;

        if (objectEntity != entt::null) {
          const auto& object = checked_get<comp::Object>(registry, objectEntity);

          auto& drag = registry.emplace<comp::ObjectDragInfo>(objectEntity);
          drag.origin_object_pos = object.pos;
          drag.last_mouse_pos = mouse.pos;
        }

        break;
      }
      case cen::mouse_button::right: {
        active.object = objectEntity;

        if (objectEntity != entt::null) {
          dispatcher.enqueue<SpawnObjectContextMenuEvent>(objectEntity);
        }

        break;
      }
      default:
        break;
    }
  }
}

void ObjectSelectionTool::on_dragged(DocumentModel& model,
                                     entt::dispatcher& dispatcher,
                                     const MouseInfo& mouse)
{
  auto& registry = model.get_active_registry();
  if (mouse.button == cen::mouse_button::left && sys::is_object_layer_active(registry)) {
    const auto& active = ctx_get<comp::ActiveState>(registry);
    if (active.object != entt::null) {
      if (auto* drag = registry.try_get<comp::ObjectDragInfo>(active.object)) {
        auto& object = checked_get<comp::Object>(registry, active.object);
        if (mouse.is_within_contents) {
          const auto ratio = sys::get_viewport_scaling_ratio(registry);
          const auto delta = (mouse.pos - drag->last_mouse_pos) / ratio;

          object.pos += delta;
          drag->last_mouse_pos = mouse.pos;
        }
        else {
          /* Stop if the user drags the object outside the map */
          maybe_emit_event(model, dispatcher);
        }
      }
    }
  }
}

void ObjectSelectionTool::on_released(DocumentModel& model,
                                      entt::dispatcher& dispatcher,
                                      const MouseInfo& mouse)
{
  auto& registry = model.get_active_registry();
  if (mouse.button == cen::mouse_button::left && sys::is_object_layer_active(registry)) {
    maybe_emit_event(model, dispatcher);
  }
}

auto ObjectSelectionTool::is_available(const DocumentModel& model) const -> bool
{
  const auto& registry = model.get_active_registry();
  return sys::is_object_layer_active(registry);
}

auto ObjectSelectionTool::get_type() const -> ToolType
{
  return ToolType::ObjectSelection;
}

void ObjectSelectionTool::maybe_emit_event(DocumentModel& model,
                                           entt::dispatcher& dispatcher)
{
  auto& registry = model.get_active_registry();
  const auto& active = ctx_get<comp::ActiveState>(registry);
  if (active.object != entt::null) {
    if (const auto* drag = registry.try_get<comp::ObjectDragInfo>(active.object)) {
      const auto& object = checked_get<comp::Object>(registry, active.object);

      /* Only emit an event if the object has been moved along any axis */
      if (drag->origin_object_pos != object.pos) {
        const auto& context = checked_get<comp::Context>(registry, active.object);
        dispatcher.enqueue<MoveObjectEvent>(context.id,
                                            drag->origin_object_pos,
                                            object.pos);
      }

      registry.remove<comp::ObjectDragInfo>(active.object);
    }
  }
}

}  // namespace tactile