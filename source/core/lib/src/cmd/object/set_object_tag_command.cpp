// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/object/set_object_tag_command.hpp"

#include <utility>  // move, exchange

#include "tactile/base/document/document.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/layer/layer_types.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

SetObjectTagCommand::SetObjectTagCommand(IDocument* document,
                                         const EntityID object_id,
                                         std::string tag)
  : m_document {require_not_null(document, "null document")},
    m_object_id {object_id},
    m_new_tag {std::move(tag)},
    m_old_tag {}
{}

void SetObjectTagCommand::undo()
{
  TACTILE_CORE_TRACE("Reverting tag of object {} to {}",
                    entity_to_string(m_object_id),
                    m_old_tag);

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(m_object_id);

  object.tag = std::move(m_old_tag);
  m_old_tag.clear();
}

void SetObjectTagCommand::redo()
{
  TACTILE_CORE_TRACE("Changing tag of object {} to {}",
                    entity_to_string(m_object_id),
                    m_new_tag);

  auto& registry = m_document->get_registry();
  auto& object = registry.get<CObject>(m_object_id);

  m_old_tag = std::exchange(object.tag, m_new_tag);
}

auto SetObjectTagCommand::merge_with(const ICommand* cmd) -> bool
{
  const auto* other = dynamic_cast<const SetObjectTagCommand*>(cmd);

  if (!other || other->m_document != m_document || other->m_object_id != m_object_id) {
    return false;
  }

  m_new_tag = other->m_new_tag;

  return true;
}

}  // namespace tactile::core
