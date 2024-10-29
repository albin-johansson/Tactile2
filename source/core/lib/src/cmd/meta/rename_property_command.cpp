// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/cmd/meta/rename_property_command.hpp"

#include <utility>  // move

#include "tactile/base/container/lookup.hpp"
#include "tactile/base/document/document.hpp"
#include "tactile/core/debug/assert.hpp"
#include "tactile/base/debug/validation.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/meta/meta.hpp"

namespace tactile::core {

RenamePropertyCommand::RenamePropertyCommand(IDocument* document,
                                             const EntityID context_id,
                                             std::string old_name,
                                             std::string new_name)
  : m_document {require_not_null(document, "null document")},
    m_context_id {context_id},
    m_old_name {std::move(old_name)},
    m_new_name {std::move(new_name)}
{}

void RenamePropertyCommand::undo()
{
  _rename_property(m_new_name, m_old_name);
}

void RenamePropertyCommand::redo()
{
  _rename_property(m_old_name, m_new_name);
}

auto RenamePropertyCommand::merge_with(const ICommand* cmd) -> bool
{
  const auto* other = dynamic_cast<const RenamePropertyCommand*>(cmd);
  if (!other) {
    return false;
  }

  const auto can_merge = m_document == other->m_document &&
                         m_context_id == other->m_context_id &&
                         m_new_name == other->m_old_name;
  if (!can_merge) {
    return false;
  }

  m_new_name = other->m_new_name;

  return true;
}

void RenamePropertyCommand::_rename_property(const std::string_view from, std::string to)
{
  TACTILE_CORE_TRACE("Renaming property '{}' to '{}'", from, to);

  auto& registry = m_document->get_registry();
  auto& meta = registry.get<CMeta>(m_context_id);

  TACTILE_ASSERT(exists_in(meta.properties, from));
  TACTILE_ASSERT(!exists_in(meta.properties, to));

  auto property = take_from(meta.properties, from).value();
  meta.properties.insert_or_assign(std::move(to), std::move(property));
}

}  // namespace tactile::core
