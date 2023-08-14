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
#include "core/tiles/tile_pos.hpp"
#include "tactile/core/common/prelude.hpp"
#include "tactile/core/containers/vector.hpp"
#include "tactile/core/functional/maybe.hpp"

namespace tactile::cmd {

class BucketFill final : public Command {
 public:
  BucketFill(Registry* registry,
             Entity tile_layer_entity,
             TilePos origin,
             TileID replacement);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Registry* mRegistry;
  Entity mTileLayerEntity;
  TilePos mOrigin;
  TileID mReplacement;
  Maybe<TileID> mTargetTileID;
  Vector<TilePos> mAffectedPositions;
};

}  // namespace tactile::cmd
