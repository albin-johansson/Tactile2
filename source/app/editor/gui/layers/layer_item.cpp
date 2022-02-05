#include "layer_item.hpp"

#include <imgui.h>

#include "core/components/attribute_context.hpp"
#include "core/components/layer.hpp"
#include "core/components/parent.hpp"
#include "core/utils/formatted_string.hpp"
#include "editor/events/layer_events.hpp"
#include "editor/gui/icons.hpp"
#include "editor/gui/scoped.hpp"
#include "layer_item_popup.hpp"
#include "tactile.hpp"

namespace tactile {
namespace {

void GroupLayerItem(const entt::registry& registry,
                    const icon_manager& icons,
                    entt::dispatcher& dispatcher,
                    const entt::entity layerEntity,
                    const comp::layer& layer,
                    const ImGuiTreeNodeFlags flags,
                    const c_str name)
{
  ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
  if (scoped::TreeNode treeNode{"##GroupLayerTreeNode", flags, "%s", name};
      treeNode.IsOpen()) {
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    if (ImGui::IsItemActivated() ||
        (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
      dispatcher.enqueue<select_layer_event>(layer.id);
    }

    UpdateLayerItemPopup(registry, dispatcher, layer.id);

    const auto& node = registry.get<comp::layer_tree_node>(layerEntity);
    for (const auto child : node.children) {
      const auto& childLayer = registry.get<comp::layer>(child);
      LayerItem(registry, icons, dispatcher, child, childLayer);
    }
  }
  else {
    ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

    if (ImGui::IsItemActivated() ||
        (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
      dispatcher.enqueue<select_layer_event>(layer.id);
    }

    UpdateLayerItemPopup(registry, dispatcher, layer.id);
  }
}

}  // namespace

void LayerItem(const entt::registry& registry,
               const icon_manager& icons,
               entt::dispatcher& dispatcher,
               const entt::entity layerEntity,
               const comp::layer& layer)
{
  const scoped::ID scope{layer.id};

  const auto& activeLayer = registry.ctx<comp::active_layer>();
  const auto isActiveLayer = layerEntity == activeLayer.entity;

  auto flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
               ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |
               ImGuiTreeNodeFlags_SpanFullWidth;
  if (isActiveLayer) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }

  const auto& context = registry.get<comp::attribute_context>(layerEntity);
  formatted_string name{"{} {}", icons.get_icon(layer.type), context.name};

  if (layer.type != layer_type::group_layer) {
    if (ImGui::Selectable(name.data(), isActiveLayer)) {
      dispatcher.enqueue<select_layer_event>(layer.id);
    }

    /* Make sure to select the layer item when right-clicked as well */
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
      dispatcher.enqueue<select_layer_event>(layer.id);
    }

    UpdateLayerItemPopup(registry, dispatcher, layer.id);
  }
  else {
    GroupLayerItem(registry, icons, dispatcher, layerEntity, layer, flags, name.data());
  }
}

}  // namespace tactile
