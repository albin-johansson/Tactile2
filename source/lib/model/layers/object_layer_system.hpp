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

#include "core/ecs/entity.hpp"
#include "core/ecs/registry.hpp"
#include "core/math/vector.hpp"
#include "core/tiles/tile_pos.hpp"
#include "model/layers/layer_components.hpp"

namespace tactile::sys {

[[nodiscard]] auto find_object_at_position(const Registry& registry,
                                           const ObjectLayer& object_layer,
                                           const Float2& target_pos,
                                           const Float2& tile_size) -> Entity;

}  // namespace tactile::sys
