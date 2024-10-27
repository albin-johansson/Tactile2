// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/create_layer_command.hpp"

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_common.hpp"
#include "tactile/core/layer/tile_layer.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

CreateLayerCommand::CreateLayerCommand(MapDocument* document, const LayerType type)
  : m_document {require_not_null(document, "null document")},
    m_type {type},
    m_parent_layer_id {kInvalidEntity},
    m_layer_id {kInvalidEntity},
    m_layer_was_added {false}
{}

void CreateLayerCommand::undo()
{
  TACTILE_CORE_DEBUG("Removing layer {}", entity_to_string(m_layer_id));

  auto& registry = m_document->get_registry();

  const auto& document_info = registry.get<CDocumentInfo>();
  const auto map_id = document_info.root;

  remove_layer_from_map(registry, map_id, m_layer_id).value();

  m_layer_was_added = false;
}

void CreateLayerCommand::redo()
{
  auto& registry = m_document->get_registry();

  const auto& document_info = registry.get<CDocumentInfo>();
  const auto map_id = document_info.root;

  auto& map = registry.get<CMap>(map_id);

  if (m_layer_id == kInvalidEntity) {
    m_parent_layer_id = map.active_layer;
    m_layer_id = add_layer_to_map(registry, map_id, m_type).value();
  }
  else {
    const auto old_active_layer = map.active_layer;
    map.active_layer = m_parent_layer_id;

    append_layer_to_map(registry, map_id, m_layer_id);

    map.active_layer = old_active_layer;
  }

  TACTILE_CORE_DEBUG("Created layer {}", entity_to_string(m_layer_id));
  m_layer_was_added = true;
}

void CreateLayerCommand::dispose()
{
  if (!m_layer_was_added && m_layer_id != kInvalidEntity) {
    auto& registry = m_document->get_registry();
    destroy_layer(registry, m_layer_id);

    m_layer_id = kInvalidEntity;
  }
}

}  // namespace tactile::core
