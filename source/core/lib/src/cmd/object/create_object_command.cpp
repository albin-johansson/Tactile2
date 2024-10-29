// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/create_object_command.hpp"

#include "tactile/base/debug/validation.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_document.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/layer/object.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"

namespace tactile::core {

CreateObjectCommand::CreateObjectCommand(MapDocument* document,
                                         const EntityID layer_id,
                                         const ObjectType type,
                                         const Float2& position,
                                         const Float2& size)
  : m_document {require_not_null(document, "null document")},
    m_layer_id {layer_id},
    m_type {type},
    m_position {position},
    m_size {size},
    m_object_id {kInvalidEntity},
    m_object_was_added {false}
{}

void CreateObjectCommand::undo()
{
  TACTILE_CORE_TRACE("Removing object {}", entity_to_string(m_object_id));
  auto& registry = m_document->get_registry();

  auto& object_layer = registry.get<CObjectLayer>(m_layer_id);
  std::erase(object_layer.objects, m_object_id);

  m_object_was_added = false;
}

void CreateObjectCommand::redo()
{
  auto& registry = m_document->get_registry();

  const auto map_id = registry.get<CDocumentInfo>().root;
  auto& id_cache = registry.get<CMapIdCache>(map_id);

  if (m_object_id == kInvalidEntity) {
    m_object_id = make_object(registry, id_cache.next_object_id++, m_type);

    auto& object = registry.get<CObject>(m_object_id);
    object.position = m_position;
    object.size = m_size;
  }

  auto& object_layer = registry.get<CObjectLayer>(m_layer_id);
  object_layer.objects.push_back(m_object_id);

  TACTILE_CORE_TRACE("Created object {}", entity_to_string(m_object_id));
  m_object_was_added = true;
}

void CreateObjectCommand::dispose()
{
  if (!m_object_was_added && m_object_id != kInvalidEntity) {
    auto& registry = m_document->get_registry();
    destroy_object(registry, m_object_id);

    m_object_id = kInvalidEntity;
  }
}

}  // namespace tactile::core
