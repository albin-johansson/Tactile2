// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/tactile_app.hpp"

#include <stdexcept>  // runtime_error
#include <utility>    // move

#include <imgui.h>

#include "tactile/base/debug/validation.hpp"
#include "tactile/base/render/renderer.hpp"
#include "tactile/base/render/window.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/ui/common/style.hpp"
#include "tactile/core/ui/i18n/language_parser.hpp"

namespace tactile::core {

TactileApp::TactileApp(IRuntime* runtime)
  : m_runtime {require_not_null(runtime, "null runtime")},
    m_window {require_not_null(runtime->get_window(), "null window")},
    m_renderer {require_not_null(runtime->get_renderer(), "null renderer")},
    m_settings {},
    m_language {},
    m_model {},
    m_widget_manager {},
    m_event_dispatcher {},
    m_file_event_handler {},
    m_edit_event_handler {},
    m_view_event_handler {},
    m_tileset_event_handler {},
    m_map_event_handler {},
    m_layer_event_handler {},
    m_object_event_handler {},
    m_component_event_handler {},
    m_property_event_handler {},
    m_viewport_event_handler {}
{}

TactileApp::~TactileApp() noexcept = default;

void TactileApp::on_startup()
{
  auto& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ui::apply_custom_style(ImGui::GetStyle());

  m_window->show();
  m_window->maximize();

  m_settings = get_default_settings();

  if (auto language = ui::parse_language_from_disk(m_settings.language)) {
    language->add_icons();
    m_language.emplace(std::move(*language));
  }
  else {
    TACTILE_CORE_ERROR("Could not parse language file: {}", to_string(language.error()));
    throw std::runtime_error {"could not parse language file"};
  }

  auto& model = m_model.emplace(&m_settings, &m_language.value());

  auto& file_event_handler = m_file_event_handler.emplace(&model, m_runtime);
  auto& edit_event_handler = m_edit_event_handler.emplace(&model);
  auto& view_event_handler =
      m_view_event_handler.emplace(&model, m_renderer, &m_widget_manager);
  auto& tileset_event_handler =
      m_tileset_event_handler.emplace(&model, m_renderer, &m_widget_manager);
  auto& map_event_handler = m_map_event_handler.emplace(&model, &m_widget_manager, m_runtime);
  auto& layer_event_handler = m_layer_event_handler.emplace(&model);
  auto& object_event_handler = m_object_event_handler.emplace(&model);
  auto& component_event_handler = m_component_event_handler.emplace(&model, &m_widget_manager);
  auto& property_event_handler = m_property_event_handler.emplace(&model, &m_widget_manager);
  auto& viewport_event_handler = m_viewport_event_handler.emplace(&model);

  file_event_handler.install(m_event_dispatcher);
  edit_event_handler.install(m_event_dispatcher);
  view_event_handler.install(m_event_dispatcher);
  tileset_event_handler.install(m_event_dispatcher);
  map_event_handler.install(m_event_dispatcher);
  layer_event_handler.install(m_event_dispatcher);
  object_event_handler.install(m_event_dispatcher);
  component_event_handler.install(m_event_dispatcher);
  property_event_handler.install(m_event_dispatcher);
  viewport_event_handler.install(m_event_dispatcher);
}

void TactileApp::on_shutdown()
{
  m_window->hide();
}

void TactileApp::on_update()
{
  m_event_dispatcher.update();
}

void TactileApp::on_render()
{
  m_widget_manager.push(*m_model, m_event_dispatcher);
}

void TactileApp::on_framebuffer_scale_changed(const float framebuffer_scale)
{
  m_event_dispatcher.push<ReloadFontsEvent>(framebuffer_scale);
}

}  // namespace tactile::core
