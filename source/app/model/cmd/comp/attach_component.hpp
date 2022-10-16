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

#include "core/type/ptr.hpp"
#include "core/uuid.hpp"
#include "model/cmd/command.hpp"

namespace tactile {
class Context;
class ComponentIndex;
}  // namespace tactile

namespace tactile::cmd {

/// Command for attaching a component to a context.
class AttachComponent final : public Command {
 public:
  AttachComponent(Shared<ComponentIndex> index,
                  Shared<Context> context,
                  const UUID& component_id);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Shared<ComponentIndex> mIndex;
  Shared<Context> mContext;
  UUID mComponentId {};
};

}  // namespace tactile::cmd