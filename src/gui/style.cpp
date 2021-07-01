#include "style.hpp"

#include "imgui.h"

namespace Tactile {

void ApplyStyle(ImGuiStyle& style)
{
  style.WindowBorderSize = 0;

  style.WindowPadding = ImVec2{10, 10};
  style.FramePadding = ImVec2{5, 5};
  style.CellPadding = ImVec2{4, 4};

  style.ItemSpacing = ImVec2{10, 8};
  style.ItemInnerSpacing = ImVec2{4, 6};
  style.IndentSpacing = 15;

  style.GrabMinSize = 6;
  style.ScrollbarSize = 10;

  const float rounding = 2;
  style.WindowRounding = rounding;
  style.FrameRounding = rounding;
  style.PopupRounding = rounding;
  style.ScrollbarRounding = rounding;
  style.GrabRounding = rounding;
  style.TabRounding = rounding;
}

}  // namespace Tactile
