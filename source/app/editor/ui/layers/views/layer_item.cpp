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

#include "layer_item.hpp"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <imgui.h>

#include "core/common/ecs.hpp"
#include "core/components/attributes.hpp"
#include "core/components/layers.hpp"
#include "core/components/parent.hpp"
#include "core/events/layer_events.hpp"
#include "core/events/property_events.hpp"
#include "core/systems/layers/layer_system.hpp"
#include "core/systems/layers/layer_tree_system.hpp"
#include "core/utils/formatted_string.hpp"
#include "editor/ui/icons.hpp"
#include "editor/ui/scoped.hpp"
#include "misc/panic.hpp"

namespace tactile {
namespace {

constexpr int _base_node_flags =
    ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
    ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |
    ImGuiTreeNodeFlags_SpanFullWidth;

[[nodiscard]] auto _get_icon(const LayerType type) -> const char*
{
  switch (type) {
    case LayerType::TileLayer:
      return TAC_ICON_TILE_LAYER;

    case LayerType::ObjectLayer:
      return TAC_ICON_OBJECT_LAYER;

    case LayerType::GroupLayer:
      return TAC_ICON_GROUP_LAYER;

    default:
      throw TactileError("Failed to recognize layer type!");
  }
}

void _update_layer_item_popup(const entt::registry& registry,
                              entt::dispatcher& dispatcher,
                              const LayerID id)
{
  if (auto popup = scoped::Popup::for_item("##LayerItemPopup"); popup.is_open()) {
    const auto layerEntity = sys::get_layer(registry, id);
    const auto& layer = checked_get<comp::Layer>(registry, layerEntity);

    if (ImGui::MenuItem(TAC_ICON_INSPECT " Inspect Layer")) {
      dispatcher.enqueue<InspectContextEvent>(layerEntity);
    }

    ImGui::Separator();
    if (ImGui::MenuItem(TAC_ICON_EDIT " Rename Layer")) {
      dispatcher.enqueue<OpenRenameLayerDialogEvent>(id);
    }

    ImGui::Separator();
    if (ImGui::MenuItem(TAC_ICON_DUPLICATE " Duplicate Layer")) {
      dispatcher.enqueue<DuplicateLayerEvent>(id);
    }

    ImGui::Separator();
    if (ImGui::MenuItem(TAC_ICON_REMOVE " Remove Layer")) {
      dispatcher.enqueue<RemoveLayerEvent>(id);
    }

    ImGui::Separator();
    if (ImGui::MenuItem(TAC_ICON_VISIBILITY " Toggle Layer Visibility",
                        nullptr,
                        layer.visible)) {
      dispatcher.enqueue<SetLayerVisibleEvent>(id, !layer.visible);
    }

    if (auto opacity = layer.opacity; ImGui::SliderFloat("Opacity", &opacity, 0, 1.0f)) {
      dispatcher.enqueue<SetLayerOpacityEvent>(id, opacity);
    }

    ImGui::Separator();
    if (ImGui::MenuItem(TAC_ICON_MOVE_UP " Move Layer Up",
                        nullptr,
                        false,
                        sys::can_move_layer_up(registry, layerEntity))) {
      dispatcher.enqueue<MoveLayerUpEvent>(id);
    }

    if (ImGui::MenuItem(TAC_ICON_MOVE_DOWN " Move Layer Down",
                        nullptr,
                        false,
                        sys::can_move_layer_down(registry, layerEntity))) {
      dispatcher.enqueue<MoveLayerDownEvent>(id);
    }
  }
}

void _show_group_layer_item(const entt::registry& registry,
                            entt::dispatcher& dispatcher,
                            const entt::entity layerEntity,
                            const comp::Layer& layer,
                            const ImGuiTreeNodeFlags flags,
                            const char* name)
{
  ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
  if (scoped::TreeNode treeNode{"##GroupLayerTreeNode", flags, "%s", name};
      treeNode.is_open()) {
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    if (ImGui::IsItemActivated() ||
        (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
      dispatcher.enqueue<SelectLayerEvent>(layer.id);
    }

    _update_layer_item_popup(registry, dispatcher, layer.id);

    const auto& node = checked_get<comp::LayerTreeNode>(registry, layerEntity);
    for (const auto child : node.children) {
      layer_item_view(registry, dispatcher, child);
    }
  }
  else {
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    if (ImGui::IsItemActivated() ||
        (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
      dispatcher.enqueue<SelectLayerEvent>(layer.id);
    }

    _update_layer_item_popup(registry, dispatcher, layer.id);
  }
}

}  // namespace

void layer_item_view(const entt::registry& registry,
                     entt::dispatcher& dispatcher,
                     const entt::entity layerEntity)
{
  const auto& layer = checked_get<comp::Layer>(registry, layerEntity);
  const auto& activeLayer = ctx_get<comp::ActiveLayer>(registry);

  const scoped::Id scope{layer.id};

  const auto isActiveLayer = layerEntity == activeLayer.entity;
  const auto flags = isActiveLayer ? (_base_node_flags | ImGuiTreeNodeFlags_Selected)  //
                                   : _base_node_flags;

  const auto& context = checked_get<comp::AttributeContext>(registry, layerEntity);
  const FormattedString name{"{} {}", _get_icon(layer.type), context.name};

  if (layer.type != LayerType::GroupLayer) {
    if (ImGui::Selectable(name.data(), isActiveLayer)) {
      dispatcher.enqueue<SelectLayerEvent>(layer.id);
    }

    /* Make sure to select the layer item when right-clicked as well */
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      dispatcher.enqueue<SelectLayerEvent>(layer.id);
    }

    _update_layer_item_popup(registry, dispatcher, layer.id);
  }
  else {
    _show_group_layer_item(registry, dispatcher, layerEntity, layer, flags, name.data());
  }
}

}  // namespace tactile