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

#include "core/map.hpp"
#include "core/tile_pos.hpp"
#include "core/type/maybe.hpp"
#include "core/type/ptr.hpp"
#include "core/type/vector.hpp"
#include "core/uuid.hpp"
#include "core/vocabulary.hpp"
#include "model/cmd/command.hpp"

namespace tactile::cmd {

class BucketFill final : public Command {
 public:
  BucketFill(Shared<Map> map,
             const UUID& layer_id,
             const TilePos& origin,
             TileID replacement);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto get_name() const -> String override;

 private:
  Shared<Map> mMap;
  UUID mLayerId {};
  TilePos mOrigin;
  TileID mReplacement {};
  Maybe<TileID> mTarget;
  Vec<TilePos> mPositions;
};

}  // namespace tactile::cmd