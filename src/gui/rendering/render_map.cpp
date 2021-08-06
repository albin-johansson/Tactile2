#include "render_map.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include "core/map/layers/layer_utils.hpp"
#include "core/map_document.hpp"
#include "io/preferences.hpp"
#include "render_bounds.hpp"
#include "render_info.hpp"
#include "render_object_layer.hpp"
#include "render_tile_layer.hpp"

namespace Tactile {
namespace {

constexpr auto out_border_color = IM_COL32(0xFF, 0xFF, 0xFF, 0xFF);
constexpr auto in_border_color = IM_COL32(0xFF, 0xFF, 0xFF, 50);

void RenderOutline(const RenderInfo& info)
{
  const auto width = info.col_count * info.grid_size.x;
  const auto height = info.row_count * info.grid_size.y;
  ImGui::GetWindowDrawList()->AddRect(info.map_position,
                                      info.map_position + ImVec2{width, height},
                                      out_border_color);
}

void RenderGrid(const RenderInfo& info)
{
  auto* drawList = ImGui::GetWindowDrawList();

  const auto& bounds = info.bounds;
  const auto endRow = bounds.end.GetRow();
  const auto endCol = bounds.end.GetColumn();
  for (auto row = bounds.begin.GetRow(); row < endRow; ++row)
  {
    for (auto col = bounds.begin.GetColumn(); col < endCol; ++col)
    {
      const ImVec2 pos = {
          info.map_position.x + (info.grid_size.x * static_cast<float>(col)),
          info.map_position.y + (info.grid_size.y * static_cast<float>(row))};
      drawList->AddRect(pos, pos + info.grid_size, in_border_color);
    }
  }
}

void RenderLayer(const TilesetManager& tilesets,
                 const SharedLayer& layer,
                 const RenderInfo& info,
                 const float parentOpacity)
{
  if (const auto* tileLayer = AsTileLayer(layer))
  {
    RenderTileLayer(*tileLayer, tilesets, info, parentOpacity);
  }
  else if (const auto* objectLayer = AsObjectLayer(layer))
  {
    RenderObjectLayer(*objectLayer, info, parentOpacity);
  }
  else if (const auto* groupLayer = AsGroupLayer(layer))
  {
    if (groupLayer->IsVisible())
    {
      for (const auto& [subid, sublayer] : *groupLayer)
      {
        const auto opacity = parentOpacity * groupLayer->GetOpacity();
        RenderLayer(tilesets, sublayer, info, opacity);
      }
    }
  }
}

}  // namespace

void RenderMap(const MapDocument& document, const RenderInfo& info)
{
  const auto& tilesets = document.GetTilesets();
  for (const auto& [id, layer] : document.GetMap())
  {
    if (layer->IsVisible())
    {
      RenderLayer(tilesets, layer, info, layer->GetOpacity());
    }
  }

  if (Prefs::GetShowGrid())
  {
    RenderGrid(info);
  }

  RenderOutline(info);
}

}  // namespace Tactile
