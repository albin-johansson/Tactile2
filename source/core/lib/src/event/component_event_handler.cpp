// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/component_event_handler.hpp"

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/widget_manager.hpp"

namespace tactile::core {

ComponentEventHandler::ComponentEventHandler(Model* model, ui::WidgetManager* widget_manager)
  : mModel {require_not_null(model, "null model")},
    mWidgetManager {require_not_null(widget_manager, "null widget manager")}
{}

void ComponentEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = ComponentEventHandler;

  // clang-format off
  dispatcher.bind<ShowComponentEditorDialogEvent, &Self::on_show_component_editor_dialog>(this);
  dispatcher.bind<CreateComponentEvent, &Self::on_create_component>(this);
  // clang-format on
}

void ComponentEventHandler::on_show_component_editor_dialog(
    const ShowComponentEditorDialogEvent&)
{
  TACTILE_CORE_TRACE("ShowComponentEditorDialogEvent");
  // TODO
}

void ComponentEventHandler::on_create_component(const CreateComponentEvent&)
{
  TACTILE_CORE_TRACE("CreateComponentEvent");
  // TODO
}

// TODO UndefComponentEvent
// TODO RenameComponentEvent
// TODO UpdateComponentEvent
// TODO AttachComponentEvent
// TODO DetachComponentEvent
// TODO UpdateAttachedComponentEvent
// TODO ResetAttachedComponentEvent
// TODO AddComponentAttrEvent
// TODO RemoveComponentAttrEvent
// TODO RenameComponentAttrEvent
// TODO DuplicateComponentAttrEvent
// TODO SetComponentAttrTypeEvent

}  // namespace tactile::core
