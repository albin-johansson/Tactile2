// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/set_object_visibility_command.hpp"

#include <utility>  // exchange

#include "tactile/base/document/document.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

SetObjectVisibilityCommand::SetObjectVisibilityCommand(IDocument* document,
                                                       const EntityID object_id,
                                                       const bool visibility)
  : m_document {require_not_null(document, "null document")},
    m_object_id {object_id},
    m_new_visibility {visibility},
    m_old_visibility {false}
{}

void SetObjectVisibilityCommand::undo()
{
  TACTILE_CORE_TRACE("Setting visibility of object {} to {}",
                    entity_to_string(m_object_id),
                    m_old_visibility);

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(m_object_id);

  object.is_visible = m_old_visibility;
}

void SetObjectVisibilityCommand::redo()
{
  TACTILE_CORE_TRACE("Setting visibility of object {} to {}",
                    entity_to_string(m_object_id),
                    m_new_visibility);

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(m_object_id);

  m_old_visibility = std::exchange(object.is_visible, m_new_visibility);
}

}  // namespace tactile::core
