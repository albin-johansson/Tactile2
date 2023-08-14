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
#include "core/ecs/entity.hpp"
#include "core/ecs/registry.hpp"
#include "tactile/core/common/prelude.hpp"
#include "tactile/core/contexts/property.hpp"
#include "tactile/core/functional/maybe.hpp"

namespace tactile::cmd {

class UpdateProperty final : public Command {
 public:
  UpdateProperty(Registry* registry,
                 Entity context_entity,
                 String name,
                 Property new_value);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto merge_with(const Command* cmd) -> bool override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Registry* mRegistry;
  Entity mContextEntity;
  String mName;
  Property mNewValue;
  Maybe<Property> mOldValue;
  Maybe<usize> mUpdatedVectorComponentIndex;
};

}  // namespace tactile::cmd
