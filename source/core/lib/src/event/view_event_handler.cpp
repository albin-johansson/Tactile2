// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/view_event_handler.hpp"

#include <imgui.h>

#include "tactile/base/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/model/settings.hpp"
#include "tactile/core/ui/widget_manager.hpp"

namespace tactile::core {

ViewEventHandler::ViewEventHandler(Model* model,
                                   IRenderer* renderer,
                                   ui::WidgetManager* widget_manager)
  : mModel {require_not_null(model, "null model")},
    mRenderer {require_not_null(renderer, "null renderer")},
    mWidgetManager {require_not_null(widget_manager, "null widget manager")}
{}

void ViewEventHandler::install(EventDispatcher& dispatcher)
{
  TACTILE_CORE_DEBUG("Installing view event handler");

  using Self = ViewEventHandler;

  // clang-format off
  dispatcher.bind<ResetLayoutEvent, &Self::on_reset_layout>(this);
  dispatcher.bind<TogglePropertyDockEvent, &Self::on_toggle_property_dock>(this);
  dispatcher.bind<ToggleComponentDockEvent, &Self::on_toggle_component_dock>(this);
  dispatcher.bind<ToggleLayerDockEvent, &Self::on_toggle_layer_dock>(this);
  dispatcher.bind<ToggleTilesetDockEvent, &Self::on_toggle_tileset_dock>(this);
  dispatcher.bind<ToggleAnimationDockEvent, &Self::on_toggle_animation_dock>(this);
  dispatcher.bind<ToggleLogDockEvent, &Self::on_toggle_log_dock>(this);
  dispatcher.bind<SetThemeEvent, &Self::on_set_theme>(this);
  dispatcher.bind<IncreaseFontSizeEvent, &Self::on_increase_font_size>(this);
  dispatcher.bind<DecreaseFontSizeEvent, &Self::on_decrease_font_size>(this);
  dispatcher.bind<ResetFontSizeEvent, &Self::on_reset_font_size>(this);
  dispatcher.bind<ReloadFontsEvent, &Self::on_reload_fonts>(this);
  dispatcher.bind<SetFontEvent, &Self::on_set_font>(this);
  dispatcher.bind<ToggleGridEvent, &Self::on_toggle_grid>(this);
  dispatcher.bind<ToggleLayerHighlightEvent, &Self::on_toggle_layer_highlight>(this);
  // clang-format on
}

void ViewEventHandler::on_reset_layout(const ResetLayoutEvent&)
{
  TACTILE_CORE_TRACE("ResetLayoutEvent");

  auto& dock_space = mWidgetManager->get_dock_space();
  dock_space.reset_layout(mModel->get_language());
}

void ViewEventHandler::on_toggle_property_dock(const TogglePropertyDockEvent&)
{
  TACTILE_CORE_TRACE("TogglePropertyDockEvent");
  // TODO
}

void ViewEventHandler::on_toggle_component_dock(const ToggleComponentDockEvent&)
{
  TACTILE_CORE_TRACE("ToggleComponentDockEvent");
  // TODO
}

void ViewEventHandler::on_toggle_layer_dock(const ToggleLayerDockEvent&)
{
  TACTILE_CORE_TRACE("ToggleLayerDockEvent");
  // TODO
}

void ViewEventHandler::on_toggle_tileset_dock(const ToggleTilesetDockEvent&)
{
  TACTILE_CORE_TRACE("ToggleTilesetDockEvent");
  // TODO
}

void ViewEventHandler::on_toggle_animation_dock(const ToggleAnimationDockEvent&)
{
  TACTILE_CORE_TRACE("ToggleAnimationDockEvent");
  // TODO
}

void ViewEventHandler::on_toggle_log_dock(const ToggleLogDockEvent&)
{
  TACTILE_CORE_TRACE("ToggleLogDockEvent");
  // TODO
}

void ViewEventHandler::on_set_theme(const SetThemeEvent&)
{
  TACTILE_CORE_TRACE("SetThemeEvent");
  // TODO
}

void ViewEventHandler::on_increase_font_size(const IncreaseFontSizeEvent&)
{
  TACTILE_CORE_TRACE("IncreaseFontSizeEvent");
  // TODO
}

void ViewEventHandler::on_decrease_font_size(const DecreaseFontSizeEvent&)
{
  TACTILE_CORE_TRACE("DecreaseFontSizeEvent");
  // TODO
}

void ViewEventHandler::on_reset_font_size(const ResetFontSizeEvent&)
{
  TACTILE_CORE_TRACE("ResetFontSizeEvent");
  // TODO
}

void ViewEventHandler::on_set_font(const SetFontEvent& event)
{
  TACTILE_CORE_TRACE("SetFontEvent");

  auto& settings = mModel->get_settings();
  settings.font = event.font;

  const auto& io = ImGui::GetIO();
  on_reload_fonts(ReloadFontsEvent {
    .framebuffer_scale = io.DisplayFramebufferScale.x,
  });
}

void ViewEventHandler::on_reload_fonts(const ReloadFontsEvent& event)
{
  TACTILE_CORE_TRACE("ReloadFontsEvent(framebuffer scale: {})", event.framebuffer_scale);
  ui::reload_fonts(*mRenderer, mModel->get_settings(), event.framebuffer_scale);
}

void ViewEventHandler::on_toggle_grid(const ToggleGridEvent&)
{
  TACTILE_CORE_TRACE("ToggleGridEvent");
  // TODO
}

void ViewEventHandler::on_toggle_layer_highlight(const ToggleLayerHighlightEvent&)
{
  TACTILE_CORE_TRACE("ToggleLayerHighlightEvent");
  // TODO
}

}  // namespace tactile::core
