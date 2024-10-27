// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/event/layer_event_handler.hpp"

#include <magic_enum.hpp>

#include "tactile/core/cmd/layer/create_layer_command.hpp"
#include "tactile/core/cmd/layer/duplicate_layer_command.hpp"
#include "tactile/core/cmd/layer/move_layer_down_command.hpp"
#include "tactile/core/cmd/layer/move_layer_up_command.hpp"
#include "tactile/core/cmd/layer/remove_layer_command.hpp"
#include "tactile/core/cmd/layer/set_layer_opacity_command.hpp"
#include "tactile/core/cmd/layer/set_layer_visibility_command.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/event/event_dispatcher.hpp"
#include "tactile/core/event/events.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/model/model.hpp"

namespace tactile::core {

LayerEventHandler::LayerEventHandler(Model* model)
  : mModel {require_not_null(model, "null model")}
{}

void LayerEventHandler::install(EventDispatcher& dispatcher)
{
  using Self = LayerEventHandler;
  dispatcher.bind<CreateLayerEvent, &Self::on_create_layer>(this);
  dispatcher.bind<RemoveLayerEvent, &Self::on_remove_layer>(this);
  dispatcher.bind<DuplicateLayerEvent, &Self::on_duplicate_layer>(this);
  dispatcher.bind<MoveLayerUpEvent, &Self::on_move_layer_up>(this);
  dispatcher.bind<MoveLayerDownEvent, &Self::on_move_layer_down>(this);
  dispatcher.bind<RenameLayerEvent, &Self::on_rename_layer>(this);
  dispatcher.bind<SetLayerOpacityEvent, &Self::on_set_layer_opacity>(this);
  dispatcher.bind<SetLayerVisibleEvent, &Self::on_set_layer_visible>(this);
}

void LayerEventHandler::on_create_layer(const CreateLayerEvent& event)
{
  TACTILE_CORE_TRACE("CreateLayerEvent(type: {})", magic_enum::enum_name(event.type));
  mModel->push_map_command<CreateLayerCommand>(event.type);
}

void LayerEventHandler::on_remove_layer(const RemoveLayerEvent& event)
{
  TACTILE_CORE_TRACE("RemoveLayerEvent");
  mModel->push_map_command<RemoveLayerCommand>(event.layer_entity);
}

void LayerEventHandler::on_duplicate_layer(const DuplicateLayerEvent& event)
{
  TACTILE_CORE_TRACE("DuplicateLayerEvent");
  mModel->push_map_command<DuplicateLayerCommand>(event.layer_entity);
}

void LayerEventHandler::on_move_layer_up(const MoveLayerUpEvent& event)
{
  TACTILE_CORE_TRACE("MoveLayerUpEvent");
  mModel->push_map_command<MoveLayerUpCommand>(event.layer_entity);
}

void LayerEventHandler::on_move_layer_down(const MoveLayerDownEvent& event)
{
  TACTILE_CORE_TRACE("MoveLayerDownEvent");
  mModel->push_map_command<MoveLayerDownCommand>(event.layer_entity);
}

void LayerEventHandler::on_rename_layer(const RenameLayerEvent&)
{
  TACTILE_CORE_TRACE("RenameLayerEvent");
  // TODO
}

void LayerEventHandler::on_set_layer_opacity(const SetLayerOpacityEvent& event)
{
  TACTILE_CORE_TRACE("SetLayerOpacityEvent");
  mModel->push_map_command<SetLayerOpacityCommand>(event.layer_entity, event.opacity);
}

void LayerEventHandler::on_set_layer_visible(const SetLayerVisibleEvent& event)
{
  TACTILE_CORE_TRACE("SetLayerVisibleEvent");
  mModel->push_map_command<SetLayerVisibilityCommand>(event.layer_entity, event.visible);
}

}  // namespace tactile::core
