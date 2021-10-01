#include "render_stamp_preview.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <cassert>  // assert

#include "common/ints.hpp"
#include "core/components/texture.hpp"
#include "core/components/tileset.hpp"
#include "core/components/uv_tile_size.hpp"
#include "core/map_position.hpp"
#include "core/systems/map_system.hpp"
#include "gui/rendering/render_info.hpp"
#include "gui/texture_utils.hpp"

namespace Tactile {
namespace {

constexpr uint32 preview_opacity = 100;  // [0, 255]

struct PreviewInfo final
{
  ImTextureID texture_id{};

  ImVec2 map_pos{};
  ImVec2 grid_size{};
  ImVec2 uv_size{};

  MapPosition mouse_pos{};
  MapPosition selection_begin;
  MapPosition selection_size;
  MapPosition offset;
};

void RenderPreviewTile(const PreviewInfo& info,
                       const MapPosition& previewTilePos,
                       const ImVec2& tilesetTilePos)
{
  const auto x = static_cast<float>(previewTilePos.GetColumn()) * info.grid_size.x;
  const auto y = static_cast<float>(previewTilePos.GetRow()) * info.grid_size.y;
  const auto realPos = info.map_pos + ImVec2{x, y};

  const auto uvMin = tilesetTilePos * info.uv_size;
  const auto uvMax = uvMin + info.uv_size;

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddImage(info.texture_id,
                     realPos,
                     realPos + info.grid_size,
                     uvMin,
                     uvMax,
                     IM_COL32(0xFF, 0xFF, 0xFF, preview_opacity));
}

void RenderPreviewTiles(const entt::registry& registry, const PreviewInfo& info)
{
  const auto endRow = info.selection_size.GetRow();
  const auto endCol = info.selection_size.GetColumn();
  for (auto row = 0; row < endRow; ++row) {
    for (auto col = 0; col < endCol; ++col) {
      const auto position = MapPosition{row, col};
      const auto previewTilePos = info.mouse_pos + position - info.offset;

      if (Sys::IsPositionInMap(registry, previewTilePos)) {
        const auto tilesetTilePos = info.selection_begin + position;
        const auto tilesetTileRow = static_cast<float>(tilesetTilePos.GetRow());
        const auto tilesetTileCol = static_cast<float>(tilesetTilePos.GetColumn());
        RenderPreviewTile(info, previewTilePos, {tilesetTileCol, tilesetTileRow});
      }
    }
  }
}

}  // namespace

void RenderStampPreview(const entt::registry& registry,
                        const MapPosition& mousePos,
                        const RenderInfo& renderInfo)
{
  const auto& activeTileset = registry.ctx<ActiveTileset>();

  const auto tilesetEntity = activeTileset.entity;
  assert(tilesetEntity != entt::null);

  const auto& selection = registry.get<TilesetSelection>(tilesetEntity);
  if (!selection.region) {
    return;
  }

  const auto& region = selection.region.value();
  const auto& texture = registry.get<Texture>(tilesetEntity);
  const auto& uv = registry.get<UvTileSize>(tilesetEntity);

  PreviewInfo info;
  info.texture_id = ToTextureID(texture.id);

  info.map_pos = renderInfo.map_position;
  info.grid_size = renderInfo.grid_size;
  info.uv_size = {uv.width, uv.height};

  info.mouse_pos = mousePos;
  info.selection_begin = region.begin;
  info.selection_size = region.end - region.begin;
  info.offset = info.selection_size / MapPosition{2, 2};

  RenderPreviewTiles(registry, info);
}

}  // namespace Tactile
