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

#include "core/cmd/command.hpp"
#include "core/common/memory.hpp"
#include "core/common/tile_cache.hpp"
#include "core/common/uuid.hpp"
#include "core/map.hpp"

namespace tactile::cmd {

class EraserSequence final : public Command {
 public:
  EraserSequence(Shared<Map> map, const UUID& layer_id, TileCache old_state);

  void undo() override;

  void redo() override;

  [[nodiscard]] auto get_name() const -> std::string override;

 private:
  Shared<Map> mMap;
  UUID mLayerId {};
  TileCache mOldState;
};

}  // namespace tactile::cmd
