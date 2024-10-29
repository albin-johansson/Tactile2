// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/update_property_command.hpp"

#include <utility>  // move, exchange

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

UpdatePropertyCommand::UpdatePropertyCommand(IDocument* document,
                                             const EntityID context_id,
                                             std::string property_name,
                                             Attribute property_value)
  : m_document {require_not_null(document, "null document")},
    m_context_id {context_id},
    m_property_name {std::move(property_name)},
    m_new_property_value {std::move(property_value)},
    m_old_property_value {}
{}

void UpdatePropertyCommand::undo()
{
  TACTILE_CORE_TRACE("Reverting update of property '{}'", m_property_name);

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  auto& value = lookup_in(meta.properties, m_property_name);
  value = m_old_property_value;
}

void UpdatePropertyCommand::redo()
{
  TACTILE_CORE_TRACE("Updating the value of property '{}'", m_property_name);

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  auto& value = lookup_in(meta.properties, m_property_name);
  m_old_property_value = std::exchange(value, m_new_property_value);
}

auto UpdatePropertyCommand::merge_with(const ICommand* cmd) -> bool
{
  const auto* other = dynamic_cast<const UpdatePropertyCommand*>(cmd);
  if (!other) {
    return false;
  }

  const auto same_target = m_document == other->m_document &&
                           m_context_id == other->m_context_id &&
                           m_property_name == other->m_property_name;
  if (!same_target) {
    return false;
  }

  m_new_property_value = other->m_new_property_value;
  return true;
}

}  // namespace tactile::core
