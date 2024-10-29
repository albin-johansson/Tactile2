// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/move_object_command.hpp"

#include <utility>  // exchange

#include "tactile/base/document/document.hpp"
#include "tactile/base/numeric/vec_format.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

MoveObjectCommand::MoveObjectCommand(IDocument* document,
                                     const EntityID object_id,
                                     const Float2& position)
  : m_document {require_not_null(document, "null document")},
    m_object_id {object_id},
    m_new_position {position},
    m_old_position {0, 0}
{}

void MoveObjectCommand::undo()
{
  TACTILE_CORE_TRACE("Moving object {} to {}", entity_to_string(m_object_id), m_old_position);
  auto& registry = m_document->get_registry();

  auto& object = registry.get<CObject>(m_object_id);
  object.position = m_old_position;
}

void MoveObjectCommand::redo()
{
  TACTILE_CORE_TRACE("Moving object {} to {}", entity_to_string(m_object_id), m_new_position);
  auto& registry = m_document->get_registry();

  auto& object = registry.get<CObject>(m_object_id);
  m_old_position = std::exchange(object.position, m_new_position);
}

}  // namespace tactile::core
