/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "cmd/command.hpp"
#include "common/attribute.hpp"
#include "common/type/ecs.hpp"
#include "common/type/maybe.hpp"
#include "model/model.hpp"

namespace tactile::cmd {

/// Command for changing the default value of an attribute in a component.
class UpdateComponent final : public Command {
 public:
  UpdateComponent(Model* model,
                  Entity component_entity,
                  String attribute_name,
                  Attribute new_value);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto merge_with(const Command* cmd) -> bool override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Model* mModel;
  Entity mComponentEntity;
  String mAttributeName;
  Attribute mNewValue;
  Maybe<Attribute> mOldValue;
};

}  // namespace tactile::cmd