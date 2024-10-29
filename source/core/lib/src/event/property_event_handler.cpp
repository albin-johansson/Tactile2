// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/property_event_handler.hpp"

#include "tactile/core/cmd/meta/create_property_command.hpp"
#include "tactile/core/cmd/meta/remove_property_command.hpp"
#include "tactile/core/cmd/meta/rename_property_command.hpp"
#include "tactile/core/cmd/meta/update_property_command.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"
#include "tactile/core/ui/widget_manager.hpp"

namespace tactile::core {

PropertyEventHandler::PropertyEventHandler(Model* model, ui::WidgetManager* widget_manager)
  : mModel {require_not_null(model, "null model")},
    mWidgetManager {require_not_null(widget_manager, "null widget manager")}
{}

void PropertyEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = PropertyEventHandler;

  // clang-format off
  dispatcher.bind<ShowNewPropertyDialogEvent, &Self::on_show_new_property_dialog>(this);
  dispatcher.bind<ShowRenamePropertyDialogEvent, &Self::on_show_rename_property_dialog>(this);
  dispatcher.bind<CreatePropertyEvent, &Self::on_create_property>(this);
  dispatcher.bind<UpdatePropertyEvent, &Self::on_update_property>(this);
  dispatcher.bind<RemovePropertyEvent, &Self::on_remove_property>(this);
  dispatcher.bind<RenamePropertyEvent, &Self::on_rename_property>(this);
  // clang-format on
}

void PropertyEventHandler::on_show_new_property_dialog(const ShowNewPropertyDialogEvent& event)
{
  TACTILE_CORE_TRACE("ShowNewPropertyDialogEvent(context: {})",
                    entity_to_string(event.context_entity));
  mWidgetManager->get_new_property_dialog().open(event.context_entity);
}

void PropertyEventHandler::on_show_rename_property_dialog(
    const ShowRenamePropertyDialogEvent& event)
{
  TACTILE_CORE_TRACE("ShowRenamePropertyDialogEvent(context: {}, name: {})",
                    entity_to_string(event.context_entity),
                    event.name);
  mWidgetManager->get_rename_property_dialog().open(event.context_entity, event.name);
}

void PropertyEventHandler::on_create_property(const CreatePropertyEvent& event)
{
  TACTILE_CORE_TRACE("CreatePropertyEvent(context: {}, name: {})",
                    entity_to_string(event.context_entity),
                    event.name);
  mModel->push_command<CreatePropertyCommand>(event.context_entity, event.name, event.value);
}

void PropertyEventHandler::on_update_property(const UpdatePropertyEvent& event)
{
  TACTILE_CORE_TRACE("UpdatePropertyEvent(context: {}, name: {})",
                    entity_to_string(event.context_entity),
                    event.name);
  mModel->push_command<UpdatePropertyCommand>(event.context_entity, event.name, event.value);
}

void PropertyEventHandler::on_remove_property(const RemovePropertyEvent& event)
{
  TACTILE_CORE_TRACE("RemovePropertyEvent(context: {}, name: {})",
                    entity_to_string(event.context_entity),
                    event.name);
  mModel->push_command<RemovePropertyCommand>(event.context_entity, event.name);
}

void PropertyEventHandler::on_rename_property(const RenamePropertyEvent& event)
{
  TACTILE_CORE_TRACE("RenamePropertyEvent(context: {}, old name: {}, new name: {})",
                    entity_to_string(event.context_entity),
                    event.old_name,
                    event.new_name);
  mModel->push_command<RenamePropertyCommand>(event.context_entity,
                                              event.old_name,
                                              event.new_name);
}

}  // namespace tactile::core
