// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/object_event_handler.hpp"

#include "tactile/core/cmd/object/create_object_command.hpp"
#include "tactile/core/cmd/object/move_object_command.hpp"
#include "tactile/core/cmd/object/remove_object_command.hpp"
#include "tactile/core/cmd/object/set_object_tag_command.hpp"
#include "tactile/core/cmd/object/set_object_visibility_command.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"

namespace tactile::core {

ObjectEventHandler::ObjectEventHandler(Model* model)
  : m_model {require_not_null(model, "null model")}
{}

void ObjectEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = ObjectEventHandler;
  dispatcher.bind<CreateObjectEvent, &Self::on_create_object>(this);
  dispatcher.bind<RemoveObjectEvent, &Self::on_remove_object>(this);
  dispatcher.bind<MoveObjectEvent, &Self::on_move_object>(this);
  dispatcher.bind<SetObjectTagEvent, &Self::on_set_object_tag>(this);
  dispatcher.bind<SetObjectVisibleEvent, &Self::on_set_object_visible>(this);
}

void ObjectEventHandler::on_create_object(const CreateObjectEvent& event) const
{
  TACTILE_CORE_TRACE("CreateObjectEvent");
  m_model->push_map_command<CreateObjectCommand>(event.layer_id,
                                                 event.type,
                                                 event.position,
                                                 event.size);
}

void ObjectEventHandler::on_remove_object(const RemoveObjectEvent& event) const
{
  TACTILE_CORE_TRACE("RemoveObjectEvent");
  m_model->push_map_command<RemoveObjectCommand>(event.layer_id, event.object_id);
}

void ObjectEventHandler::on_move_object(const MoveObjectEvent& event) const
{
  TACTILE_CORE_TRACE("MoveObjectEvent");
  m_model->push_map_command<MoveObjectCommand>(event.object_id, event.position);
}

void ObjectEventHandler::on_set_object_tag(const SetObjectTagEvent& event) const
{
  TACTILE_CORE_TRACE("SetObjectTagEvent");
  m_model->push_map_command<SetObjectTagCommand>(event.object_id, event.tag);
}

void ObjectEventHandler::on_set_object_visible(const SetObjectVisibleEvent& event) const
{
  TACTILE_CORE_TRACE("SetObjectVisibleEvent");
  m_model->push_map_command<SetObjectVisibilityCommand>(event.object_id, event.visible);
}

}  // namespace tactile::core
