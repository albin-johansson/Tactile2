#include "tileset_viewport_widget.hpp"

#include <algorithm>  // min, max

#include "core/events/set_tileset_selection_event.hpp"
#include "core/tileset/tileset.hpp"
#include "gui/canvas_info.hpp"
#include "gui/get_texture_id.hpp"
#include "gui/show_grid.hpp"
#include "gui/widgets/mouse_tracker.hpp"
#include "gui/widgets/rubber_band.hpp"
#include "imgui.h"
#include "imgui_internal.h"

namespace Tactile {
namespace {

inline constexpr auto rubber_band_color = IM_COL32(0, 0x44, 0xCC, 100);

inline GridState state;

void ClampOffsets(const ImVec2 canvasSize,
                  const float width,
                  const float height)
{
  state.scroll_offset.x = std::min(0.0f, state.scroll_offset.x);
  state.scroll_offset.y = std::min(0.0f, state.scroll_offset.y);

  state.scroll_offset.x =
      std::max(-width + canvasSize.x, state.scroll_offset.x);
  state.scroll_offset.y =
      std::max(-height + canvasSize.y, state.scroll_offset.y);
}

}  // namespace

void TilesetViewportWidget(const Tileset& tileset, entt::dispatcher& dispatcher)
{
  const auto texture = GetTextureID(tileset.GetTexture());
  const auto textureSize = ImVec2{static_cast<float>(tileset.GetWidth()),
                                  static_cast<float>(tileset.GetHeight())};
  const auto tileSize = ImVec2{static_cast<float>(tileset.GetTileWidth()),
                               static_cast<float>(tileset.GetTileHeight())};
  state.grid_size = tileSize;

  const auto info = GetCanvasInfo();
  FillBackground(info);

  MouseTracker(info, state);
  ClampOffsets(info.canvas_size, textureSize.x, textureSize.y);

  if (const auto selection = RubberBand(state.scroll_offset, tileSize))
  {
    dispatcher.enqueue<SetTilesetSelectionEvent>(*selection);
  }

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->PushClipRect(info.canvas_tl, info.canvas_br, true);

  const auto min = drawList->GetClipRectMin() + state.scroll_offset;
  drawList->AddImage(texture, min, min + textureSize);

  if (const auto& selection = tileset.GetSelection())
  {
    const auto tl = selection->top_left;
    const auto br = selection->bottom_right;
    const auto diff = br - tl;

    const ImVec2 origin{static_cast<float>(tl.GetColumn()) * tileSize.x,
                        static_cast<float>(tl.GetRow()) * tileSize.y};

    const ImVec2 size{static_cast<float>(diff.GetColumn()) * tileSize.x,
                      static_cast<float>(diff.GetRow()) * tileSize.y};

    drawList->AddRectFilled(min + origin,
                            min + origin + size,
                            rubber_band_color);
  }

  ShowGrid(state, info);
  drawList->PopClipRect();
}

}  // namespace Tactile
