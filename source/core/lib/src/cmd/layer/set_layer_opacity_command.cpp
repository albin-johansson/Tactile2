// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/layer/set_layer_opacity_command.hpp"

#include <utility>  // exchange

#include "tactile/core/debug/validation.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

SetLayerOpacityCommand::SetLayerOpacityCommand(MapDocument* document,
                                               const EntityID layer_id,
                                               const float opacity)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id},
    m_new_opacity {opacity},
    m_old_opacity {0.0f}
{}

void SetLayerOpacityCommand::undo()
{
  TACTILE_CORE_TRACE("Setting layer {} opacity to {}",
                    entity_to_string(m_layer_id),
                    m_old_opacity);
  auto& registry = m_document->get_registry();

  auto& layer = registry.get<CLayer>(m_layer_id);
  layer.opacity = m_old_opacity;
}

void SetLayerOpacityCommand::redo()
{
  TACTILE_CORE_TRACE("Setting layer {} opacity to {}",
                    entity_to_string(m_layer_id),
                    m_new_opacity);
  auto& registry = m_document->get_registry();

  auto& layer = registry.get<CLayer>(m_layer_id);
  m_old_opacity = std::exchange(layer.opacity, m_new_opacity);
}

auto SetLayerOpacityCommand::merge_with(const ICommand* cmd) -> bool
{
  const auto* other = dynamic_cast<const SetLayerOpacityCommand*>(cmd);

  if (!other || m_document != other->m_document || m_layer_id != other->m_layer_id) {
    return false;
  }

  m_new_opacity = other->m_new_opacity;

  return true;
}

}  // namespace tactile::core
