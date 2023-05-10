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

#include "move_animation_frame_backwards.hpp"

#include "common/debug/assert.hpp"
#include "common/util/vector.hpp"
#include "core/tile.hpp"
#include "lang/language.hpp"
#include "lang/strings.hpp"
#include "model/model.hpp"

namespace tactile::cmd {

MoveAnimationFrameBackwards::MoveAnimationFrameBackwards(const Entity tile_entity,
                                                         const usize frame_index)
    : mTileEntity {tile_entity},
      mFrameIndex {frame_index}
{
}

void MoveAnimationFrameBackwards::undo()
{
  auto& model = get_model();
  auto& animation = model.get<TileAnimation>(mTileEntity);

  TACTILE_ASSERT(mFrameIndex + 1 < animation.frames.size());

  const TileAnimationFrame frame {
      .tile_index = animation.frames.at(mFrameIndex + 1).tile_index,
      .duration = animation.frames.at(mFrameIndex + 1).duration,
  };

  erase_at(animation.frames, mFrameIndex + 1);
  insert_at(animation.frames, mFrameIndex, frame);
}

void MoveAnimationFrameBackwards::redo()
{
  auto& model = get_model();
  auto& animation = model.get<TileAnimation>(mTileEntity);

  TACTILE_ASSERT(mFrameIndex + 1 < animation.frames.size());

  const TileAnimationFrame frame {
      .tile_index = animation.frames.at(mFrameIndex).tile_index,
      .duration = animation.frames.at(mFrameIndex).duration,
  };

  erase_at(animation.frames, mFrameIndex);
  insert_at(animation.frames, mFrameIndex + 1, frame);
}

auto MoveAnimationFrameBackwards::get_name() const -> String
{
  const auto& lang = get_current_language();
  return lang.cmd.move_animation_frame_backwards;
}

}  // namespace tactile::cmd