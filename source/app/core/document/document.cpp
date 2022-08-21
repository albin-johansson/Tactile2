/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "document.hpp"

#include <utility>  // move

#include "core/cmd/command_stack.hpp"
#include "core/cmd/comp/all.hpp"
#include "core/cmd/property/all.hpp"
#include "core/ctx/context_manager.hpp"

namespace tactile {

void ADocument::define_component(std::string name)
{
  get_history().exec<DefineComponentCmd>(get_component_index(), std::move(name));
}

void ADocument::undef_component(const UUID& component_id)
{
  get_history().exec<UndefComponentCmd>(this, component_id);
}

void ADocument::rename_component(const UUID& component_id, std::string name)
{
  get_history().exec<RenameComponentCmd>(get_component_index(),
                                         component_id,
                                         std::move(name));
}

void ADocument::add_component_attribute(const UUID& component_id, std::string name)
{
  get_history().exec<AddComponentAttrCmd>(this, component_id, std::move(name));
}

void ADocument::remove_component_attribute(const UUID& component_id, std::string name)
{
  get_history().exec<RemoveComponentAttrCmd>(this, component_id, std::move(name));
}

void ADocument::rename_component_attribute(const UUID& component_id,
                                           std::string current,
                                           std::string updated)
{
  get_history().exec<RenameComponentAttrCmd>(this,
                                             component_id,
                                             std::move(current),
                                             std::move(updated));
}

void ADocument::duplicate_component_attribute(const UUID& component_id, std::string name)
{
  get_history().exec<DuplicateComponentAttrCmd>(this, component_id, std::move(name));
}

void ADocument::set_component_attribute_type(const UUID&         component_id,
                                             std::string         name,
                                             const AttributeType type)
{
  get_history().exec<SetComponentAttrTypeCmd>(this, component_id, std::move(name), type);
}

void ADocument::update_component(const UUID& component_id,
                                 std::string name,
                                 Attribute   value)
{
  get_history().exec<UpdateComponentCmd>(get_component_index(),
                                         component_id,
                                         std::move(name),
                                         std::move(value));
}

void ADocument::attach_component(const UUID& context_id, const UUID& component_id)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<AttachComponentCmd>(get_component_index(),
                                         std::move(context),
                                         component_id);
}

void ADocument::detach_component(const UUID& context_id, const UUID& component_id)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<DetachComponentCmd>(std::move(context), component_id);
}

void ADocument::update_attached_component(const UUID& context_id,
                                          const UUID& component_id,
                                          std::string name,
                                          Attribute   value)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<UpdateAttachedComponentCmd>(std::move(context),
                                                 component_id,
                                                 std::move(name),
                                                 std::move(value));
}

void ADocument::reset_attached_component(const UUID& context_id, const UUID& component_id)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<ResetAttachedComponentCmd>(get_component_index(),
                                                std::move(context),
                                                component_id);
}

void ADocument::add_property(const UUID&         context_id,
                             std::string         name,
                             const AttributeType type)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<cmd::AddProperty>(std::move(context), std::move(name), type);
}

void ADocument::remove_property(const UUID& context_id, std::string name)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<cmd::RemoveProperty>(std::move(context), std::move(name));
}

void ADocument::rename_property(const UUID& context_id,
                                std::string current,
                                std::string updated)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<cmd::RenameProperty>(std::move(context),
                                          std::move(current),
                                          std::move(updated));
}

void ADocument::update_property(const UUID& context_id, std::string name, Attribute value)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<cmd::UpdateProperty>(std::move(context),
                                          std::move(name),
                                          std::move(value));
}

void ADocument::change_property_type(const UUID&         context_id,
                                     std::string         name,
                                     const AttributeType type)
{
  auto context = get_contexts().get_context(context_id);
  get_history().exec<cmd::ChangePropertyType>(std::move(context), std::move(name), type);
}

auto ADocument::is_map() const -> bool
{
  return get_type() == DocumentType::Map;
}

auto ADocument::is_tileset() const -> bool
{
  return get_type() == DocumentType::Tileset;
}

}  // namespace tactile