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

#include "fonts.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <spdlog/spdlog.h>

#include "io/directories.hpp"
#include "io/persist/preferences.hpp"

namespace tactile::ui {
namespace {

constexpr auto _roboto_path = "assets/fonts/roboto/Roboto-Regular.ttf";
constexpr auto _fa_path = "assets/fonts/fa/fa-solid-900.otf";
constexpr ImWchar _icon_range[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

}  // namespace

void reload_fonts()
{
  static const auto roboto = io::find_resource(_roboto_path).string();
  static const auto fa = io::find_resource(_fa_path).string();

  spdlog::debug("Reloading fonts...");

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

  /* The global scale is 1 on most platforms, and 0.5 on macOS */
  io.FontGlobalScale = 1.0f / scale.x;

  ImFontConfig config {};
  config.MergeMode = true;
  config.GlyphMinAdvanceX = size * scale.x;
  config.GlyphMaxAdvanceX = config.GlyphMinAdvanceX;
  config.GlyphOffset = {0, 2};
  io.Fonts->AddFontFromFileTTF(fa.c_str(), size * scale.x, &config, _icon_range);

  io.Fonts->Build();

  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();

  ImGui::GetStyle().ScaleAllSizes(1.0f);
}

auto can_increase_font_size() -> bool
{
  const auto& prefs = io::get_preferences();
  return !prefs.use_default_font && prefs.font_size < max_font_size;
}

auto can_decrease_font_size() -> bool
{
  const auto& prefs = io::get_preferences();
  return !prefs.use_default_font && prefs.font_size > min_font_size;
}

}  // namespace tactile::ui