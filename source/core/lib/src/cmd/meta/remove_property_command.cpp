// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/remove_property_command.hpp"

#include <utility>  // move

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/core/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

RemovePropertyCommand::RemovePropertyCommand(IDocument* document,
                                             const EntityID context_entity,
                                             std::string name)
  : m_document {require_not_null(document, "null document")},
    m_context_id {context_entity},
    m_name {std::move(name)},
    m_value {}
{}

void RemovePropertyCommand::undo()
{
  TACTILE_CORE_TRACE("Restoring property '{}' to {}", m_name, entity_to_string(m_context_id));

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  meta.properties.insert_or_assign(m_name, m_value);
}

void RemovePropertyCommand::redo()
{
  TACTILE_CORE_TRACE("Removing property '{}' from {}", m_name, entity_to_string(m_context_id));

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  m_value = take_from(meta.properties, m_name).value();
}

}  // namespace tactile::core
