// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include <entt/signal/dispatcher.hpp>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

#include "common/util/fmt.hpp"
#include "core/context/context_manager.hpp"
#include "handlers/event_handlers.hpp"
#include "model/document/document.hpp"
#include "model/event/property_events.hpp"
#include "model/model.hpp"
#include "runtime/app_context.hpp"
#include "ui/dock/property/dialogs/add_property_dialog.hpp"
#include "ui/dock/property/dialogs/change_property_type_dialog.hpp"
#include "ui/dock/property/dialogs/rename_property_dialog.hpp"

namespace tactile {
namespace {

void on_show_new_property_dialog(const ShowNewPropertyDialogEvent&)
{
  spdlog::trace("ShowNewPropertyDialogEvent");

  if (auto* document = get_model().active_document()) {
    const auto& context_id = document->get_contexts().active_context_id();
    ui::open_add_property_dialog(context_id);
  }
}

void on_show_rename_property_dialog(const ShowRenamePropertyDialogEvent& event)
{
  spdlog::trace("ShowRenamePropertyDialogEvent");

  if (const auto* document = get_model().active_document()) {
    const auto& context_id = document->get_contexts().active_context_id();
    ui::open_rename_property_dialog(context_id, event.current_name);
  }
}

void on_show_change_property_type_dialog(const ShowChangePropertyTypeDialogEvent& event)
{
  spdlog::trace("ShowChangePropertyTypeDialogEvent(name: {}, current_type: {})",
                event.name,
                event.current_type);

  if (const auto* document = get_model().active_document()) {
    const auto& context_id = document->get_contexts().active_context_id();
    ui::open_change_property_type_dialog(context_id, event.name, event.current_type);
  }
}

void on_add_property(const AddPropertyEvent& event)
{
  spdlog::trace("AddPropertyEvent(context_id: {}, name: {}, type: {})",
                event.context_id,
                event.name,
                event.type);

  if (auto* document = get_model().active_document()) {
    document->add_property(event.context_id, event.name, event.type);
  }
}

void on_remove_property(const RemovePropertyEvent& event)
{
  spdlog::trace("RemovePropertyEvent(context_id: {}, name: {})",
                event.context_id,
                event.name);

  if (auto* document = get_model().active_document()) {
    document->remove_property(event.context_id, event.name);
  }
}

void on_rename_property(const RenamePropertyEvent& event)
{
  spdlog::trace("RenamePropertyEvent(context_id: {}, old_name: {}, new_name: {})",
                event.context_id,
                event.old_name,
                event.new_name);

  if (auto* document = get_model().active_document()) {
    document->rename_property(event.context_id, event.old_name, event.new_name);
  }
}

void on_update_property(const UpdatePropertyEvent& event)
{
  spdlog::trace("UpdatePropertyEvent(context_id: {}, name: {}, value: {})",
                event.context_id,
                event.name,
                event.value);

  if (auto* document = get_model().active_document()) {
    document->update_property(event.context_id, event.name, event.value);
  }
}

void on_change_property_type(const ChangePropertyTypeEvent& event)
{
  spdlog::trace("ChangePropertyTypeEvent(context_id: {}, name: {}, type: {})",
                event.context_id,
                event.name,
                event.type);

  if (auto* document = get_model().active_document()) {
    document->change_property_type(event.context_id, event.name, event.type);
  }
}

void on_inspect_context(const InspectContextEvent& event)
{
  spdlog::trace("InspectContextEvent(context_id: {})", event.context_id);

  if (auto* document = get_model().active_document()) {
    document->get_contexts().select_context(event.context_id);
  }
}

}  // namespace

void install_property_event_handler()
{
  auto& dispatcher = get_dispatcher();

  // clang-format off
  dispatcher.sink<ShowNewPropertyDialogEvent>().connect<&on_show_new_property_dialog>();
  dispatcher.sink<ShowRenamePropertyDialogEvent>().connect<&on_show_rename_property_dialog>();
  dispatcher.sink<ShowChangePropertyTypeDialogEvent>().connect<&on_show_change_property_type_dialog>();
  // clang-format on

  dispatcher.sink<AddPropertyEvent>().connect<&on_add_property>();
  dispatcher.sink<RemovePropertyEvent>().connect<&on_remove_property>();
  dispatcher.sink<RenamePropertyEvent>().connect<&on_rename_property>();
  dispatcher.sink<UpdatePropertyEvent>().connect<&on_update_property>();
  dispatcher.sink<ChangePropertyTypeEvent>().connect<&on_change_property_type>();

  dispatcher.sink<InspectContextEvent>().connect<&on_inspect_context>();
}

}  // namespace tactile
