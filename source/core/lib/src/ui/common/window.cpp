// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/common/window.hpp"

#include <unordered_map>  // unordered_map

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/ui/imgui_compat.hpp"

namespace tactile::core::ui {
namespace {

/**
 * Provides window information that needs to persist between frames.
 */
struct WindowData final
{
  /** Was the mouse within the window in the previous frame? */
  bool had_mouse_then : 1;

  /** Is the mouse within the window in this frame? */
  bool has_mouse_now : 1;
};

// We only allow a single language per editor session, so this is fine.
inline std::unordered_map<const char*, WindowData> gWindowData {};

}  // namespace

Window::Window(const char* name, const ImGuiWindowFlags flags, bool* is_open)
  : mName {require_not_null(name, "null name")},
    mIsOpen {ImGui::Begin(name, is_open, flags)},
    mPos {ImGui::GetWindowPos()},
    mSize {ImGui::GetWindowSize()}
{
  auto& window_data = gWindowData[name];
  window_data.had_mouse_then = window_data.has_mouse_now;
  window_data.has_mouse_now = has_mouse();
}

Window::~Window() noexcept
{
  ImGui::End();
}

auto Window::has_mouse() const -> bool
{
  return mIsOpen && ImGui::IsMouseHoveringRect(mPos, mPos + mSize);
}

auto Window::did_mouse_enter() const -> bool
{
  const auto& window_data = lookup_in(gWindowData, mName);
  return window_data.has_mouse_now && !window_data.had_mouse_then;
}

auto Window::did_mouse_exit() const -> bool
{
  const auto& window_data = lookup_in(gWindowData, mName);
  return !window_data.has_mouse_now && window_data.had_mouse_then;
}

auto Window::is_focused(const ImGuiFocusedFlags flags) const -> bool
{
  return mIsOpen && ImGui::IsWindowFocused(flags);
}

auto Window::is_docked() const -> bool
{
  return mIsOpen && ImGui::IsWindowDocked();
}

auto Window::is_open() const -> bool
{
  return mIsOpen;
}

auto Window::get_pos() const -> Float2
{
  return to_float2(mPos);
}

auto Window::get_size() const -> Float2
{
  return to_float2(mSize);
}

auto Window::get_local_mouse_pos() const -> std::optional<Float2>
{
  if (has_mouse()) {
    return to_float2(ImGui::GetMousePos() - mPos);
  }

  return std::nullopt;
}

void center_next_window(const ImGuiCond condition)
{
  const auto* viewport = ImGui::GetMainViewport();

  const auto next_pos = viewport->WorkPos + (viewport->WorkSize * 0.5f);
  const auto next_pivot = ImVec2 {0.5f, 0.5f};

  ImGui::SetNextWindowPos(next_pos, condition, next_pivot);
}

}  // namespace tactile::core::ui
