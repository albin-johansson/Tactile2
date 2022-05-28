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

#include <string>  // string

#include "core/commands/command.hpp"
#include "core/commands/command_id.hpp"
#include "core/common/identifiers.hpp"
#include "core/common/maybe.hpp"
#include "core/components/attributes.hpp"

namespace tactile {

class CreateComponentDefCmd final : public ACommand
{
 public:
  CreateComponentDefCmd(RegistryRef registry, std::string name);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto id() const noexcept -> int override
  {
    return CommandId::create_component_def;
  }

 private:
  RegistryRef mRegistry;
  std::string mName;
  Maybe<ComponentID> mComponentId;
};

}  // namespace tactile