/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "imgui_context.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <spdlog/spdlog.h>

#include "editor/ui/style/themes.hpp"
#include "io/directories.hpp"
#include "io/proto/preferences.hpp"

namespace tactile {
namespace {

constexpr auto font_roboto_path = "assets/fonts/roboto/Roboto-Regular.ttf";
constexpr auto font_fa_path = "assets/fonts/fa/fa-solid-900.otf";
constexpr ImWchar font_icon_range[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

}  // namespace

ImGuiContext::ImGuiContext(cen::window& window, cen::gl_context& context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto& io = ImGui::GetIO();
  io.ConfigFlags |= static_cast<ImGuiConfigFlags>(ImGuiConfigFlags_DockingEnable);
  io.WantCaptureKeyboard = true;

  static const auto ini = io::widget_ini_path().string();
  io.IniFilename = ini.c_str();

  ImGui_ImplSDL2_InitForOpenGL(window.get(), context.get());
  mInitializedBackend = ImGui_ImplOpenGL3_Init();

  ImGui::StyleColorsDark();

  const auto& prefs = io::get_preferences();
  auto& style = ImGui::GetStyle();

  ui::apply_style(style);
  apply_theme(style, prefs.theme);

  style.WindowBorderSize = prefs.window_border ? 1.0f : 0.0f;

  spdlog::debug("Initialized renderer backend... {}", mInitializedBackend ? "yes" : "no");
}

ImGuiContext::~ImGuiContext()
{
  if (mInitializedBackend) {
    ImGui_ImplOpenGL3_Shutdown();
  }
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiContext::reload_fonts()
{
  static const auto roboto = io::find_resource(font_roboto_path).string();
  static const auto fa = io::find_resource(font_fa_path).string();

  spdlog::debug("[ImGuiContext] Reloading fonts...");

  auto& io = ImGui::GetIO();
  const auto scale = io.DisplayFramebufferScale;

  io.Fonts->Clear();

  const auto& prefs = io::get_preferences();
  const auto size = prefs.use_default_font ? 13.0f : static_cast<float>(prefs.font_size);

  if (prefs.use_default_font) {
    ImFontConfig config {};
    config.SizePixels = size * scale.x;
    io.Fonts->AddFontDefault(&config);
  }
  else {
    io.Fonts->AddFontFromFileTTF(roboto.c_str(), size * scale.x);
  }

  // The global scale is 1 on most platforms, and 0.5 on macOS
  io.FontGlobalScale = 1.0f / scale.x;

  ImFontConfig config {};
  config.MergeMode = true;
  config.GlyphMinAdvanceX = size * scale.x;
  config.GlyphMaxAdvanceX = config.GlyphMinAdvanceX;
  config.GlyphOffset = {0, 2};
  io.Fonts->AddFontFromFileTTF(fa.c_str(), size * scale.x, &config, font_icon_range);

  io.Fonts->Build();

  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();

  ImGui::GetStyle().ScaleAllSizes(1.0f);
}

}  // namespace tactile