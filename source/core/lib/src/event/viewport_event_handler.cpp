// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/viewport_event_handler.hpp"

#include <imgui.h>

#include "tactile/base/numeric/vec_format.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/model/settings.hpp"
#include "tactile/core/ui/imgui_compat.hpp"
#include "tactile/core/ui/viewport.hpp"

namespace tactile::core {

ViewportEventHandler::ViewportEventHandler(Model* model)
  : mModel {require_not_null(model, "null model")}
{}

void ViewportEventHandler::install(EventDispatcher& dispatcher)
{
  TACTILE_CORE_DEBUG("Installing viewport event handler");

  using Self = ViewportEventHandler;

  // clang-format off
  dispatcher.bind<OffsetViewportEvent, &Self::on_offset_viewport>(this);
  dispatcher.bind<UpdateViewportSizeEvent, &Self::on_update_viewport_size>(this);
  dispatcher.bind<UpdateViewportLimitsEvent, &Self::on_update_viewport_limits>(this);
  dispatcher.bind<IncreaseViewportZoomEvent, &Self::on_increase_viewport_zoom>(this);
  dispatcher.bind<DecreaseViewportZoomEvent, &Self::on_decrease_viewport_zoom>(this);
  dispatcher.bind<ResetViewportZoomEvent, &Self::on_reset_viewport_zoom>(this);
  dispatcher.bind<CenterViewportEvent, &Self::on_center_viewport>(this);
  dispatcher.bind<PanViewportUpEvent, &Self::on_pan_viewport_up>(this);
  dispatcher.bind<PanViewportDownEvent, &Self::on_pan_viewport_down>(this);
  dispatcher.bind<PanViewportLeftEvent, &Self::on_pan_viewport_left>(this);
  dispatcher.bind<PanViewportRightEvent, &Self::on_pan_viewport_right>(this);
  // clang-format on
}

void ViewportEventHandler::on_offset_viewport(const OffsetViewportEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("OffsetViewportEvent(viewport: {}, offset: {})",
                      entity_to_string(event.viewport_entity),
                      event.delta);
  }

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();

    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    const auto* limits = registry.find<CViewportLimits>(event.viewport_entity);

    translate_viewport(viewport, event.delta, limits);
  }
}

void ViewportEventHandler::on_update_viewport_size(const UpdateViewportSizeEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("UpdateViewportSizeEvent(viewport: {}, size: {})",
                      entity_to_string(event.viewport_entity),
                      event.new_size);
  }

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();
    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    viewport.size = event.new_size;
  }
}

void ViewportEventHandler::on_update_viewport_limits(const UpdateViewportLimitsEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("UpdateViewportLimitsEvent(viewport: {}, min: {}, max: {})",
                      entity_to_string(event.viewport_entity),
                      event.min_pos,
                      event.max_pos);
  }

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();
    auto& limits = registry.get<CViewportLimits>(event.viewport_entity);
    limits.min_pos = event.min_pos;
    limits.max_pos = event.max_pos;

    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    translate_viewport(viewport, Float2 {0, 0}, &limits);
  }
}

void ViewportEventHandler::on_increase_viewport_zoom(const IncreaseViewportZoomEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("IncreaseViewportZoomEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();

    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    const auto* limits = registry.find<CViewportLimits>(event.viewport_entity);

    increase_viewport_zoom(viewport, to_float2(ImGui::GetMousePos()), limits);
  }
}

void ViewportEventHandler::on_decrease_viewport_zoom(const DecreaseViewportZoomEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("DecreaseViewportZoomEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();

    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    const auto* limits = registry.find<CViewportLimits>(event.viewport_entity);

    decrease_viewport_zoom(viewport, to_float2(ImGui::GetMousePos()), limits);
  }
}

void ViewportEventHandler::on_reset_viewport_zoom(const ResetViewportZoomEvent& event)
{
  TACTILE_CORE_TRACE("ResetViewportZoomEvent(viewport: {})",
                    entity_to_string(event.viewport_entity));

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();
    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    viewport.scale = 1.0f;
  }
}

void ViewportEventHandler::on_center_viewport(const CenterViewportEvent& event)
{
  TACTILE_CORE_TRACE("CenterViewportEvent(viewport: {}, content_size: {})",
                    entity_to_string(event.viewport_entity),
                    event.content_size);

  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();

    auto& viewport = registry.get<CViewport>(event.viewport_entity);
    const auto* limits = registry.find<CViewportLimits>(event.viewport_entity);

    center_viewport_over_content(viewport, event.content_size, limits);
  }
}

void ViewportEventHandler::on_pan_viewport_up(const PanViewportUpEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("PanViewportUpEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }
  _pan_viewport(event.viewport_entity, Float2 {0.0f, -1.0f});
}

void ViewportEventHandler::on_pan_viewport_down(const PanViewportDownEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("PanViewportDownEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }
  _pan_viewport(event.viewport_entity, Float2 {0.0f, 1.0f});
}

void ViewportEventHandler::on_pan_viewport_left(const PanViewportLeftEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("PanViewportLeftEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }
  _pan_viewport(event.viewport_entity, Float2 {-1.0f, 0.0f});
}

void ViewportEventHandler::on_pan_viewport_right(const PanViewportRightEvent& event)
{
  if (mModel->get_settings().log_verbose_events) {
    TACTILE_CORE_TRACE("PanViewportRightEvent(viewport: {})",
                      entity_to_string(event.viewport_entity));
  }
  _pan_viewport(event.viewport_entity, Float2 {1.0f, 0.0f});
}

void ViewportEventHandler::_pan_viewport(const EntityID viewport_entity,
                                         const Float2& tile_offset_factor)
{
  if (auto* document = mModel->get_current_document()) {
    auto& registry = document->get_registry();

    auto& viewport = registry.get<CViewport>(viewport_entity);
    const auto* limits = registry.find<CViewportLimits>(viewport_entity);

    const auto tile_size = vec_cast<Float2>(document->get_tile_size());
    const auto delta = tile_size * viewport.scale * tile_offset_factor;

    translate_viewport(viewport, delta, limits);
  }
}

}  // namespace tactile::core
