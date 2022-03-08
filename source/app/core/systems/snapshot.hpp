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

#include <string>  // string
#include <vector>  // vector

#include "core/attribute.hpp"
#include "core/components/layer.hpp"
#include "core/components/object.hpp"
#include "core/components/texture.hpp"
#include "core/components/tileset.hpp"
#include "core/components/uv_tile_size.hpp"
#include "tactile.hpp"

namespace tactile::sys {

struct component_snapshot final
{
  tree_map<std::string, Attribute> attributes;
};

struct attribute_context_snapshot final
{
  context_id id{};
  std::string name;
  tree_map<std::string, Attribute> properties;
  tree_map<component_id, component_snapshot> components;
};

struct TilesetSnapshot final
{
  comp::tileset core;
  comp::tileset_selection selection;
  comp::texture texture;
  comp::uv_tile_size uv;
  attribute_context_snapshot context;
};

struct ObjectSnapshot final
{
  comp::object core;
  attribute_context_snapshot context;
};

struct LayerSnapshot final
{
  usize index{};
  comp::layer core;
  attribute_context_snapshot context;
  maybe<layer_id> parent;
  maybe<tile_matrix> tiles;
  maybe<std::vector<ObjectSnapshot>> objects;
  maybe<std::vector<LayerSnapshot>> children;
};

}  // namespace tactile::sys