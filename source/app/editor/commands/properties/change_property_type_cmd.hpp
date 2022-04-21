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

#pragma once

#include <optional>  // optional
#include <string>    // string

#include "core/attribute.hpp"
#include "editor/commands/command.hpp"
#include "editor/commands/command_id.hpp"
#include "tactile.hpp"

namespace tactile {

class ChangePropertyTypeCmd final : public ACommand
{
 public:
  ChangePropertyTypeCmd(RegistryRef registry, std::string name, AttributeType type);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto id() const noexcept -> int override
  {
    return CommandId::change_property_type;
  }

 private:
  RegistryRef mRegistry;
  ContextID mContextId;
  std::string mName;
  AttributeType mPropertyType;
  std::optional<Attribute> mPreviousValue;
};

}  // namespace tactile
