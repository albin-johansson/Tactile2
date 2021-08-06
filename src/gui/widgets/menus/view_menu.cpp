#include "view_menu.hpp"

#include <imgui.h>

#include "core/model.hpp"
#include "events/viewport/center_viewport_event.hpp"
#include "events/viewport/decrease_viewport_zoom_event.hpp"
#include "events/viewport/increase_viewport_zoom_event.hpp"
#include "events/viewport/pan_down_event.hpp"
#include "events/viewport/pan_left_event.hpp"
#include "events/viewport/pan_right_event.hpp"
#include "events/viewport/pan_up_event.hpp"
#include "events/viewport/reset_viewport_zoom_event.hpp"
#include "gui/icons.hpp"
#include "gui/layout/dock_space.hpp"
#include "io/preferences.hpp"

namespace Tactile {
namespace {

void UpdateWidgetsSubmenu(const bool hasActiveMap, entt::dispatcher& dispatcher)
{
  if (ImGui::BeginMenu("Widgets", hasActiveMap))
  {
    if (ImGui::MenuItem("Reset layout"))
    {
      ResetLayout();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Properties", nullptr, Prefs::GetShowPropertiesDock()))
    {
      Prefs::SetShowPropertiesDock(!Prefs::GetShowPropertiesDock());
    }

    if (ImGui::MenuItem("Layers", nullptr, Prefs::GetShowLayerDock()))
    {
      Prefs::SetShowLayerDock(!Prefs::GetShowLayerDock());
    }

    if (ImGui::MenuItem("Tilesets", nullptr, Prefs::GetShowTilesetDock()))
    {
      Prefs::SetShowTilesetDock(!Prefs::GetShowTilesetDock());
    }

    ImGui::EndMenu();
  }
}

}  // namespace

void UpdateViewMenu(const Model& model, entt::dispatcher& dispatcher)
{
  const auto* document = model.GetActiveDocument();

  if (ImGui::BeginMenu("View"))
  {
    UpdateWidgetsSubmenu(document != nullptr, dispatcher);

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_CENTER " Center viewport",
                        "Ctrl+Space",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<CenterViewportEvent>();
    }

    bool showGrid = Prefs::GetShowGrid();
    if (ImGui::MenuItem(TAC_ICON_GRID " Toggle grid", "Ctrl+G", &showGrid))
    {
      Prefs::SetShowGrid(showGrid);
    }

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_ZOOM_IN " Increase zoom",
                        "Ctrl+Plus",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<IncreaseViewportZoomEvent>();
    }

    if (ImGui::MenuItem(TAC_ICON_ZOOM_OUT " Decrease zoom",
                        "Ctrl+Minus",
                        false,
                        document && document->CanDecreaseViewportTileSize()))
    {
      dispatcher.enqueue<DecreaseViewportZoomEvent>();
    }

    if (ImGui::MenuItem(TAC_ICON_RESET_ZOOM " Reset zoom",
                        nullptr,
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<ResetViewportZoomEvent>();
    }

    ImGui::Separator();

    if (ImGui::MenuItem(TAC_ICON_MOVE_UP " Pan up",
                        "Ctrl+Up",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<PanUpEvent>();
    }

    if (ImGui::MenuItem(TAC_ICON_MOVE_DOWN " Pan down",
                        "Ctrl+Down",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<PanDownEvent>();
    }

    if (ImGui::MenuItem(TAC_ICON_MOVE_RIGHT " Pan right",
                        "Ctrl+Right",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<PanRightEvent>();
    }

    if (ImGui::MenuItem(TAC_ICON_MOVE_LEFT " Pan left",
                        "Ctrl+Left",
                        false,
                        document != nullptr))
    {
      dispatcher.enqueue<PanLeftEvent>();
    }

    ImGui::EndMenu();
  }
}

}  // namespace Tactile
