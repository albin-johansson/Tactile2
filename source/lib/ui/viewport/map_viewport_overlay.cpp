// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "map_viewport_overlay.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "core/layer/group_layer.hpp"
#include "core/layer/tile_layer.hpp"
#include "core/map.hpp"
#include "core/tile/tileset_bundle.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/settings.hpp"
#include "ui/widget/scoped.hpp"
#include "viewport_cursor_info.hpp"

namespace tactile::ui {
namespace {

constexpr auto kOverlayWindowFlags =
    ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
    ImGuiWindowFlags_NoMove;

constexpr float kOverlayOpacity = 0.35f;

void prepare_position_and_pivot()
{
  const auto pos = ImGui::GetWindowPos();
  const auto size = ImGui::GetWindowSize();

  const auto corner = get_settings().get_viewport_overlay_pos();
  const bool is_right =
      corner == OverlayPos::TopRight || corner == OverlayPos::BottomRight;
  const bool is_bottom =
      corner == OverlayPos::BottomLeft || corner == OverlayPos::BottomRight;

  const float padding = 10.0f;

  ImVec2 next_pos {};
  next_pos.x = is_right ? (pos.x + size.x - padding) : (pos.x + padding);
  next_pos.y = is_bottom ? (pos.y + size.y - padding)
                         : (pos.y + padding + ImGui::GetFrameHeightWithSpacing());

  ImVec2 next_pivot {};
  next_pivot.x = is_right ? 1.0f : 0.0f;
  next_pivot.y = is_bottom ? 1.0f : 0.0f;

  ImGui::SetNextWindowPos(next_pos, ImGuiCond_Always, next_pivot);
  ImGui::SetNextWindowViewport(ImGui::GetWindowViewport()->ID);
}

void show_mouse_tile_labels(const Map& map, const ViewportCursorInfo& cursor)
{
  const auto layer_id = map.get_active_layer_id();
  if (!layer_id) {
    return;
  }

  const auto& lang = get_current_language();

  if (const auto* layer = map.get_invisible_root().find_tile_layer(*layer_id)) {
    const auto tile_id = layer->tile_at(cursor.map_position);

    if (cursor.is_within_map && tile_id.has_value() && tile_id != kEmptyTile) {
      ImGui::Text("%s: %i", lang.misc.global_id.c_str(), *tile_id);

      const auto& tilesets = map.get_tileset_bundle();
      if (tilesets.is_valid_tile(*tile_id)) {
        const auto tile_index = tilesets.to_tile_index(*tile_id);
        ImGui::Text("%s: %i", lang.misc.local_id.c_str(), tile_index);
      }
      else {
        ImGui::Text("%s: -", lang.misc.local_id.c_str());
      }
    }
    else {
      ImGui::Text("%s: %s", lang.misc.global_id.c_str(), lang.misc.empty.c_str());
    }
  }
}

void update_overlay_context_menu()
{
  if (auto popup = Popup::for_window("##ViewportOverlayPopup"); popup.is_open()) {
    const auto& lang = get_current_language();

    auto& settings = get_settings();
    const auto corner = settings.get_viewport_overlay_pos();

    if (ImGui::MenuItem(lang.action.top_left.c_str(),
                        nullptr,
                        corner == OverlayPos::TopLeft)) {
      settings.set_viewport_overlay_pos(OverlayPos::TopLeft);
    }

    if (ImGui::MenuItem(lang.action.top_right.c_str(),
                        nullptr,
                        corner == OverlayPos::TopRight)) {
      settings.set_viewport_overlay_pos(OverlayPos::TopRight);
    }

    if (ImGui::MenuItem(lang.action.bottom_left.c_str(),
                        nullptr,
                        corner == OverlayPos::BottomLeft)) {
      settings.set_viewport_overlay_pos(OverlayPos::BottomLeft);
    }

    if (ImGui::MenuItem(lang.action.bottom_right.c_str(),
                        nullptr,
                        corner == OverlayPos::BottomRight)) {
      settings.set_viewport_overlay_pos(OverlayPos::BottomRight);
    }

    ImGui::Separator();

    bool show_overlay_fps = settings.test_flag(SETTINGS_SHOW_VIEWPORT_OVERLAY_FPS_BIT);
    if (ImGui::MenuItem(lang.action.show_frame_rate.c_str(),
                        nullptr,
                        &show_overlay_fps)) {
      settings.set_flag(SETTINGS_SHOW_VIEWPORT_OVERLAY_FPS_BIT, show_overlay_fps);
    }
  }
}

}  // namespace

void update_map_viewport_overlay(const Map& map, const ViewportCursorInfo& cursor)
{
  prepare_position_and_pivot();

  ImGui::SetNextWindowBgAlpha(kOverlayOpacity);
  const Window window {"##ViewportOverlay", kOverlayWindowFlags};

  if (window.is_open()) {
    const auto& lang = get_current_language();

    if (get_settings().test_flag(SETTINGS_SHOW_VIEWPORT_OVERLAY_FPS_BIT)) {
      const auto& io = ImGui::GetIO();
      ImGui::Text("%.2f ms (%.1f FPS)", 1'000.0f * io.DeltaTime, io.Framerate);
      ImGui::Separator();
    }

    if (ImGui::IsMousePosValid()) {
      ImGui::Text("X/Y: (%.0f, %.0f)",
                  cursor.scaled_position.x,
                  cursor.scaled_position.y);
    }
    else {
      ImGui::TextUnformatted("X/Y: --");
    }

    if (cursor.is_within_map) {
      ImGui::Text("%s/%s: (%i, %i)",
                  lang.misc.row.c_str(),
                  lang.misc.column.c_str(),
                  cursor.map_position.row(),
                  cursor.map_position.col());
    }
    else {
      ImGui::Text("%s/%s: --", lang.misc.row.c_str(), lang.misc.column.c_str());
    }

    ImGui::Separator();
    show_mouse_tile_labels(map, cursor);

    update_overlay_context_menu();
  }
}

}  // namespace tactile::ui
