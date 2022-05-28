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

#include "core/commands/command.hpp"
#include "core/commands/command_id.hpp"
#include "core/common/ints.hpp"
#include "core/common/maybe.hpp"
#include "map_command_cache.hpp"

namespace tactile {

class ResizeMapCmd final : public ACommand
{
 public:
  ResizeMapCmd(RegistryRef registry, usize nRows, usize nCols);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto id() const noexcept -> int override { return CommandId::resize_map; }

 private:
  RegistryRef mRegistry;
  usize mRows{};
  usize mCols{};
  MapCommandCache mCache;
  Maybe<usize> mPrevRows{};
  Maybe<usize> mPrevCols{};

  [[nodiscard]] auto is_lossy_resize() const -> bool;
};

}  // namespace tactile