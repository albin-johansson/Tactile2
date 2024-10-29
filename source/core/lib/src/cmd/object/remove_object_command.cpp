// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/remove_object_command.hpp"

#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

RemoveObjectCommand::RemoveObjectCommand(MapDocument* document,
                                         const EntityID layer_id,
                                         const EntityID object_id)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id},
    m_object_id {object_id},
    m_object_was_removed {false}
{}

void RemoveObjectCommand::undo()
{
  TACTILE_CORE_TRACE("Restoring object {}", entity_to_string(m_object_id));
  auto& registry = m_document->get_registry();

  auto& object_layer = registry.get<CObjectLayer>(m_layer_id);
  object_layer.objects.push_back(m_object_id);

  m_object_was_removed = false;
}

void RemoveObjectCommand::redo()
{
  TACTILE_CORE_TRACE("Removing object {}", entity_to_string(m_object_id));
  auto& registry = m_document->get_registry();

  auto& object_layer = registry.get<CObjectLayer>(m_layer_id);
  std::erase(object_layer.objects, m_object_id);

  m_object_was_removed = true;
}

void RemoveObjectCommand::dispose()
{
  if (m_object_was_removed && m_object_id != kInvalidEntity) {
    auto& registry = m_document->get_registry();
    destroy_object(registry, m_object_id);

    m_object_id = kInvalidEntity;
  }
}

}  // namespace tactile::core
