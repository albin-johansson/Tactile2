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

#include <entt/signal/dispatcher.hpp>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/util/fmt.hpp"
#include "handlers/event_handlers.hpp"
#include "model/document/map_document.hpp"
#include "model/event/layer_events.hpp"
#include "model/model.hpp"
#include "runtime/app_context.hpp"
#include "ui/dock/layer/layer_dock.hpp"

namespace tactile {
namespace {

void on_add_layer(const AddLayerEvent& event)
{
  spdlog::trace("AddLayerEvent(type: {})", magic_enum::enum_name(event.type));
  if (auto* document = get_model().active_map_document()) {
    document->add_layer(event.type);
  }
}

void on_remove_layer(const RemoveLayerEvent& event)
{
  spdlog::trace("RemoveLayerEvent(layer_id: {})", event.layer_id);
  if (auto* document = get_model().active_map_document()) {
    document->remove_layer(event.layer_id);
  }
}

void on_duplicate_layer(const DuplicateLayerEvent& event)
{
  spdlog::trace("DuplicateLayerEvent(layer_id: {})", event.layer_id);
  if (auto* document = get_model().active_map_document()) {
    document->duplicate_layer(event.layer_id);
  }
}

void on_select_layer(const SelectLayerEvent& event)
{
  spdlog::trace("SelectLayerEvent(layer_id: {})", event.layer_id);
  if (auto* document = get_model().active_map_document()) {
    auto& map = document->get_map();
    map.select_layer(event.layer_id);
  }
}

void on_move_layer_up(const MoveLayerUpEvent& event)
{
  spdlog::trace("MoveLayerUpEvent(layer_id: {})", event.layer_id);
  if (auto* document = get_model().active_map_document()) {
    document->move_layer_up(event.layer_id);
  }
}

void on_move_layer_down(const MoveLayerDownEvent& event)
{
  spdlog::trace("MoveLayerDownEvent(layer_id: {})", event.layer_id);
  if (auto* document = get_model().active_map_document()) {
    document->move_layer_down(event.layer_id);
  }
}

void on_set_layer_opacity(const SetLayerOpacityEvent& event)
{
  spdlog::trace("SetLayerOpacityEvent(layer_id: {}, opacity: {})",
                event.layer_id,
                event.opacity);
  if (auto* document = get_model().active_map_document()) {
    document->set_layer_opacity(event.layer_id, event.opacity);
  }
}

void on_set_layer_visible(const SetLayerVisibleEvent& event)
{
  spdlog::trace("SetLayerVisibleEvent(layer_id: {}, visible: {})",
                event.layer_id,
                event.visible);
  if (auto* document = get_model().active_map_document()) {
    document->set_layer_visible(event.layer_id, event.visible);
  }
}

void on_open_rename_layer_dialog(const OpenRenameLayerDialogEvent& event)
{
  spdlog::trace("OpenRenameLayerDialogEvent(layer_id: {})", event.layer_id);
  ui::show_rename_layer_dialog(event.layer_id);
}

void on_rename_layer(const RenameLayerEvent& event)
{
  spdlog::trace("RenameLayerEvent(layer_id: {}, name: {})", event.layer_id, event.name);
  if (auto* document = get_model().active_map_document()) {
    document->rename_layer(event.layer_id, event.name);
  }
}

}  // namespace

void install_layer_event_handler()
{
  auto& dispatcher = get_dispatcher();

  dispatcher.sink<AddLayerEvent>().connect<&on_add_layer>();
  dispatcher.sink<RemoveLayerEvent>().connect<&on_remove_layer>();
  dispatcher.sink<DuplicateLayerEvent>().connect<&on_duplicate_layer>();
  dispatcher.sink<SelectLayerEvent>().connect<&on_select_layer>();

  dispatcher.sink<MoveLayerUpEvent>().connect<&on_move_layer_up>();
  dispatcher.sink<MoveLayerDownEvent>().connect<&on_move_layer_down>();

  dispatcher.sink<SetLayerOpacityEvent>().connect<&on_set_layer_opacity>();
  dispatcher.sink<SetLayerVisibleEvent>().connect<&on_set_layer_visible>();

  dispatcher.sink<OpenRenameLayerDialogEvent>().connect<&on_open_rename_layer_dialog>();
  dispatcher.sink<RenameLayerEvent>().connect<&on_rename_layer>();
}

}  // namespace tactile