#include "viewport_cursor_info.hpp"

#include <imgui_internal.h>

#include <cmath>  // trunc

#include "gui/rendering/render_info.hpp"

namespace Tactile {

auto GetViewportCursorInfo(const RenderInfo& info) -> ViewportCursorInfo
{
  ViewportCursorInfo cursor;

  const auto mouse = ImGui::GetMousePos();
  const auto item = ImGui::GetItemRectMin();
  cursor.raw_position = mouse - info.map_position;

  const auto index = cursor.raw_position / info.grid_size;
  const auto row = std::trunc(index.y);
  const auto col = std::trunc(index.x);

  cursor.is_within_map =
      index.y >= 0 && index.x >= 0 && row < info.row_count && col < info.col_count;

  if (cursor.is_within_map)
  {
    cursor.map_position = {AsRow(row), AsColumn(col)};
  }

  cursor.clamped_position = {info.map_position.x + (col * info.grid_size.x),
                             info.map_position.y + (row * info.grid_size.y)};

  return cursor;
}

}  // namespace Tactile
