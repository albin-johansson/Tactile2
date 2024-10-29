// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/create_property_command.hpp"

#include <utility>  // move

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

CreatePropertyCommand::CreatePropertyCommand(IDocument* document,
                                             const EntityID context_id,
                                             std::string name,
                                             Attribute value)
  : m_document {require_not_null(document, "null document")},
    m_context_id {context_id},
    m_name {std::move(name)},
    m_value {std::move(value)}
{}

void CreatePropertyCommand::undo()
{
  TACTILE_CORE_TRACE("Removing property '{}' from entity {}",
                    m_name,
                    entity_to_string(m_context_id));

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  erase_from(meta.properties, m_name);
}

void CreatePropertyCommand::redo()
{
  TACTILE_CORE_TRACE("Adding property '{}' to entity {}",
                    m_name,
                    entity_to_string(m_context_id));

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  meta.properties[m_name] = m_value;
}

}  // namespace tactile::core
