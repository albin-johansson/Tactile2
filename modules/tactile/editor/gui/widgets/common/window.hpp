#pragma once

#include <tactile_def.hpp>

#include <imgui.h>

namespace Tactile {

class Window final
{
 public:
  explicit Window(const CStr label, ImGuiWindowFlags flags = 0, bool* open = nullptr)
      : mOpen{ImGui::Begin(label, open, flags)}
  {}

  ~Window()
  {
    ImGui::End();
  }

  [[nodiscard]] auto IsFocused(ImGuiFocusedFlags flags = 0) const -> bool
  {
    return mOpen && ImGui::IsWindowFocused(flags);
  }

  [[nodiscard]] static auto CurrentWindowContainsMouse() -> bool
  {
    const auto min = ImGui::GetWindowContentRegionMin();
    const auto max = ImGui::GetWindowContentRegionMax();
    return ImGui::IsMouseHoveringRect(min, max);
  }

  explicit operator bool() const noexcept
  {
    return mOpen;
  }

 private:
  bool mOpen{};
};

}  // namespace Tactile
