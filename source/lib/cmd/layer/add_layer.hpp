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
#include "core/enums/layer_type.hpp"

namespace tactile::cmd {

class AddLayer final : public Command {
 public:
  AddLayer(Registry* registry, Entity map_document_entity, LayerType type);

  void undo() override;

  void redo() override;

  void dispose() override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Registry* mRegistry;
  Entity mMapDocumentEntity;
  LayerType mLayerType;
  Entity mLayerEntity {kNullEntity};
  Entity mParentLayerEntity {kNullEntity};
  bool mLayerWasAdded : 1 {false};
};

}  // namespace tactile::cmd
