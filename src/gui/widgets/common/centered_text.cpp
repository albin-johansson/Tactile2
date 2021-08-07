#include "centered_text.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <cassert>  // assert

namespace Tactile {

void CenteredText(const NotNull<czstring> text)
{
  assert(text);
  const auto textSize = ImGui::CalcTextSize(text);

  const auto region = ImGui::GetContentRegionAvail();
  const auto halfRegion = region / ImVec2{2.0f, 2.0f};

  const auto cursorPos = ImGui::GetCursorPos();
  ImGui::SetCursorPos(cursorPos + ImVec2{halfRegion.x - (textSize.x / 2.0f), 0});
  ImGui::TextUnformatted(text);
}

}  // namespace Tactile
