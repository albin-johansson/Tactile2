// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/remove_layer_command.hpp"

#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer_common.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

RemoveLayerCommand::RemoveLayerCommand(MapDocument* document, const EntityID layer_id)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id},
    m_parent_layer_id {kInvalidEntity},
    m_layer_was_removed {false}
{}

void RemoveLayerCommand::undo()
{
  TACTILE_CORE_TRACE("Restoring layer {}", entity_to_string(m_layer_id));
  auto& registry = m_document->get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  auto& map = registry.get<CMap>(map_id);

  map.active_layer = m_parent_layer_id;
  append_layer_to_map(registry, map_id, m_layer_id);

  m_layer_was_removed = false;
}

void RemoveLayerCommand::redo()
{
  TACTILE_CORE_TRACE("Removing layer {}", entity_to_string(m_layer_id));
  auto& registry = m_document->get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  const auto& map = registry.get<CMap>(map_id);

  m_parent_layer_id = find_parent_layer(registry, map.root_layer, m_layer_id).value();

  remove_layer_from_map(registry, map_id, m_layer_id).value();
  m_layer_was_removed = true;
}

void RemoveLayerCommand::dispose()
{
  if (m_layer_was_removed && m_layer_id != kInvalidEntity) {
    auto& registry = m_document->get_registry();
    destroy_layer(registry, m_layer_id);

    m_layer_id = kInvalidEntity;
  }
}

}  // namespace tactile::core
