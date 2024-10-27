// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/ui/fonts.hpp"

#include <array>  // array

#include <IconsFontAwesome6.h>
#include <imgui.h>

#include "tactile/base/render/renderer.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/settings.hpp"

namespace tactile::core::ui {
namespace {

inline constexpr std::array<ImWchar, 3> kFontIconRange {ICON_MIN_FA, ICON_MAX_FA, 0};

}  // namespace

void reload_fonts(IRenderer& renderer, const Settings& settings, const float framebuffer_scale)
{
  if (renderer.can_reload_fonts()) {
    TACTILE_CORE_DEBUG("Reloading fonts (size: {})", settings.font_size);

    auto& io = ImGui::GetIO();
    io.Fonts->Clear();

    const auto scaled_font_size = settings.font_size * framebuffer_scale;

    if (settings.font == FontID::kDefault) {
      ImFontConfig default_config {};
      default_config.SizePixels = scaled_font_size;
      io.Fonts->AddFontDefault(&default_config);
    }
    else {
      ImFontConfig roboto_config {};
      roboto_config.SizePixels = scaled_font_size;
      io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf",
                                   scaled_font_size,
                                   &roboto_config);
    }

    ImFontConfig fa_config {};
    fa_config.MergeMode = true;
    fa_config.SizePixels = scaled_font_size;
    fa_config.GlyphMinAdvanceX = scaled_font_size;
    fa_config.GlyphMaxAdvanceX = scaled_font_size;
    fa_config.GlyphOffset = {0, 2};
    io.Fonts->AddFontFromFileTTF("assets/fonts/fa/fa-solid-900.ttf",
                                 scaled_font_size,
                                 &fa_config,
                                 kFontIconRange.data());

    // The global scale is 1 on most platforms, but 0.5 on macOS.
    io.FontGlobalScale = 1.0f / framebuffer_scale;
    io.Fonts->Build();

    renderer.try_reload_fonts();

    ImGui::GetStyle().ScaleAllSizes(1.0f);
  }
  else {
    TACTILE_CORE_WARN("Tried to reload fonts, but the renderer didn't support it");
  }
}

}  // namespace tactile::core::ui
