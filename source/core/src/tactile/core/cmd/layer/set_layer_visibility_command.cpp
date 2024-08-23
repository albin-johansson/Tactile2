// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/set_layer_visibility_command.hpp"

#include <utility>  // exchange

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer.hpp"
#include "tactile/core/log/logger.hpp"

namespace tactile {

SetLayerVisibilityCommand::SetLayerVisibilityCommand(MapDocument* document,
                                                     const EntityID layer_id,
                                                     const bool visibility) :
  m_document {require_not_null(document, "null document")},
  m_layer_id {layer_id},
  m_new_visibility {visibility},
  m_old_visibility {false}
{}

void SetLayerVisibilityCommand::undo()
{
  TACTILE_LOG_TRACE("Changing visibility of layer {} to {}",
                    entity_to_string(m_layer_id),
                    m_old_visibility);

  auto& registry = m_document->get_registry();
  auto& layer = registry.get<CLayer>(m_layer_id);

  layer.visible = m_old_visibility;
}

void SetLayerVisibilityCommand::redo()
{
  TACTILE_LOG_TRACE("Changing visibility of layer {} to {}",
                    entity_to_string(m_layer_id),
                    m_new_visibility);

  auto& registry = m_document->get_registry();
  auto& layer = registry.get<CLayer>(m_layer_id);

  m_old_visibility = std::exchange(layer.visible, m_new_visibility);
}

}  // namespace tactile
