// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/duplicate_layer_command.hpp"

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/group_layer.hpp"
#include "tactile/core/layer/layer_common.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

DuplicateLayerCommand::DuplicateLayerCommand(MapDocument* document, const EntityID layer_id)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id},
    m_duplicate_layer_id {kInvalidEntity},
    m_layer_was_added {false}
{}

void DuplicateLayerCommand::undo()
{
  TACTILE_CORE_TRACE("Removing layer {} (duplicated from {})",
                    entity_to_string(m_duplicate_layer_id),
                    entity_to_string(m_layer_id));

  auto& registry = m_document->get_registry();
  const auto map_id = registry.get<CDocumentInfo>().root;

  remove_layer_from_map(registry, map_id, m_duplicate_layer_id).value();
  m_layer_was_added = false;
}

void DuplicateLayerCommand::redo()
{
  TACTILE_CORE_TRACE("Duplicating layer {}", entity_to_string(m_layer_id));

  auto& registry = m_document->get_registry();
  const auto map_id = registry.get<CDocumentInfo>().root;

  auto& map = registry.get<CMap>(map_id);
  map.active_layer = find_parent_layer(registry, map.root_layer, m_layer_id).value();

  if (m_duplicate_layer_id == kInvalidEntity) {
    auto& id_cache = registry.get<CMapIdCache>(map_id);
    m_duplicate_layer_id = copy_layer(registry, m_layer_id, id_cache.next_layer_id);
  }

  append_layer_to_map(registry, map_id, m_duplicate_layer_id);
  m_layer_was_added = true;
}

void DuplicateLayerCommand::dispose()
{
  if (!m_layer_was_added && m_duplicate_layer_id != kInvalidEntity) {
    auto& registry = m_document->get_registry();
    destroy_layer(registry, m_duplicate_layer_id);

    m_duplicate_layer_id = kInvalidEntity;
  }
}

}  // namespace tactile::core
