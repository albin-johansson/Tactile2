#include "layer_dock.hpp"

#define NOMINMAX

#include <limits>  // numeric_limits

#include <imgui.h>
#include <tactile_def.hpp>

#include "add_layer_popup.hpp"
#include "assert.hpp"
#include "core/components/layer.hpp"
#include "core/components/parent.hpp"
#include "core/components/property_context.hpp"
#include "core/systems/layers/layer_system.hpp"
#include "editor/events/layer_events.hpp"
#include "editor/gui/alignment.hpp"
#include "editor/gui/common/button.hpp"
#include "editor/gui/common/centered_text.hpp"
#include "editor/gui/icons.hpp"
#include "editor/gui/scoped.hpp"
#include "editor/model.hpp"
#include "io/preferences.hpp"
#include "layer_item.hpp"

namespace Tactile {
namespace {

void UpdateLayerDockButtons(const entt::registry& registry, entt::dispatcher& dispatcher)
{
  const auto activeLayerEntity = registry.ctx<ActiveLayer>().entity;
  const auto hasActiveLayer = activeLayerEntity != entt::null;

  Maybe<LayerID> activeLayerId;
  if (hasActiveLayer) {
    const auto& layer = registry.get<Layer>(activeLayerEntity);
    activeLayerId = layer.id;
  }

  Scoped::Group group;

  if (Button(TAC_ICON_ADD, "Add new layer")) {
    OpenAddLayerPopup();
  }

  UpdateAddLayerPopup(dispatcher);

  if (Button(TAC_ICON_REMOVE, "Remove layer", hasActiveLayer)) {
    dispatcher.enqueue<RemoveLayerEvent>(*activeLayerId);
  }

  if (Button(TAC_ICON_DUPLICATE, "Duplicate layer", hasActiveLayer)) {
    dispatcher.enqueue<DuplicateLayerEvent>(*activeLayerId);
  }

  if (Button(TAC_ICON_MOVE_UP,
             "Move layer up",
             activeLayerId && Sys::CanMoveLayerUp(registry, *activeLayerId))) {
    dispatcher.enqueue<MoveLayerUpEvent>(*activeLayerId);
  }

  if (Button(TAC_ICON_MOVE_DOWN,
             "Move layer down",
             activeLayerId && Sys::CanMoveLayerDown(registry, *activeLayerId))) {
    dispatcher.enqueue<MoveLayerDownEvent>(*activeLayerId);
  }
}

}  // namespace

void LayerDock::Update(const Model& model,
                       const Icons& icons,
                       entt::dispatcher& dispatcher)
{
  if (!Prefs::GetShowLayerDock()) {
    return;
  }

  bool isVisible = Prefs::GetShowLayerDock();
  Scoped::Window dock{"Layers", ImGuiWindowFlags_NoCollapse, &isVisible};
  mHasFocus = dock.IsFocused(ImGuiFocusedFlags_RootAndChildWindows);

  const auto& registry = model.GetActiveRegistryRef();

  if (dock.IsOpen()) {
    UpdateLayerDockButtons(registry, dispatcher);
    ImGui::SameLine();

    Scoped::Group group;
    if (registry.view<Layer>().empty()) {
      PrepareVerticalAlignmentCenter(1);
      CenteredText("No available layers!");
    }
    else {
      const ImVec2 size{-std::numeric_limits<float>::min(),
                        -std::numeric_limits<float>::min()};
      if (Scoped::ListBox list{"##LayerTreeNode", size}; list.IsOpen()) {
        for (auto&& [entity, node] : registry.view<LayerTreeNode>().each()) {
          /* Note, we rely on the LayerTreeNode pool being sorted, so we can't include
             other components in the view query directly. */
          const auto& layer = registry.get<Layer>(entity);
          const auto& parent = registry.get<Parent>(entity);
          if (parent.entity == entt::null) {
            LayerItem(registry, icons, dispatcher, entity, layer);
          }
        }
      }
    }
  }

  Prefs::SetShowLayerDock(isVisible);

  if (mRenameTarget.has_value()) {
    const auto target = *mRenameTarget;

    const auto entity = Sys::FindLayer(registry, target);
    TACTILE_ASSERT(entity != entt::null);

    const auto& context = registry.get<PropertyContext>(entity);

    mRenameLayerDialog.Show(target, context.name);
    mRenameTarget.reset();
  }

  mRenameLayerDialog.Update(model, dispatcher);
}

void LayerDock::ShowRenameLayerDialog(const LayerID id)
{
  mRenameTarget = id;
}

}  // namespace Tactile
