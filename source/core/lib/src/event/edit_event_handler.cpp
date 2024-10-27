// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/edit_event_handler.hpp"

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"

namespace tactile::core {

EditEventHandler::EditEventHandler(Model* model)
  : mModel {require_not_null(model, "null model")}
{}

void EditEventHandler::install(EventDispatcher& dispatcher)
{
  TACTILE_CORE_DEBUG("Installing edit event handler");

  dispatcher.bind<UndoEvent, &EditEventHandler::on_undo>(this);
  dispatcher.bind<RedoEvent, &EditEventHandler::on_redo>(this);

  // TODO SelectToolEvent
  // TODO StampSequenceEvent
  // TODO EraserSequenceEvent
  // TODO FloodEvent
  // TODO AddRectangleEvent
  // TODO AddEllipseEvent
  // TODO AddPointEvent
  // TODO SetStampRandomizerEvent
}

void EditEventHandler::on_undo(const UndoEvent& event)
{
  TACTILE_CORE_TRACE("UndoEvent");
  auto& document_manager = mModel->get_document_manager();

  if (auto* document = document_manager.get_current_document()) {
    const auto& document_uuid = document->get_uuid();
    auto& command_stack = document_manager.get_history(document_uuid);
    command_stack.undo();
  }
}

void EditEventHandler::on_redo(const RedoEvent& event)
{
  TACTILE_CORE_TRACE("RedoEvent");
  auto& document_manager = mModel->get_document_manager();

  if (auto* document = document_manager.get_current_document()) {
    const auto& document_uuid = document->get_uuid();
    auto& command_stack = document_manager.get_history(document_uuid);
    command_stack.redo();
  }
}

// TODO SetCommandCapacityEvent

}  // namespace tactile::core
