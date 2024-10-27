// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/move_layer_down_command.hpp"

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

MoveLayerDownCommand::MoveLayerDownCommand(MapDocument* document, const EntityID layer_id)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id}
{}

void MoveLayerDownCommand::undo()
{
  TACTILE_CORE_TRACE("Moving layer {} up", entity_to_string(m_layer_id));
  auto& registry = m_document->get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  const auto& map = registry.get<CMap>(map_id);

  move_layer_up(registry, map.root_layer, m_layer_id);
}

void MoveLayerDownCommand::redo()
{
  TACTILE_CORE_TRACE("Moving layer {} down", entity_to_string(m_layer_id));
  auto& registry = m_document->get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  const auto& map = registry.get<CMap>(map_id);

  move_layer_down(registry, map.root_layer, m_layer_id);
}

}  // namespace tactile::core
