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

#include <cmath>        // sin, cos
#include <fstream>      // ofstream
#include <ios>          // ios
#include <numbers>      // pi
#include <string_view>  // string_view
#include <utility>      // pair, move
#include <vector>       // vector

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "core/common/associative.hpp"
#include "core/common/ints.hpp"
#include "core/common/maybe.hpp"
#include "core/tile_pos.hpp"
#include "core/util/filesystem.hpp"
#include "core/util/functional.hpp"
#include "core/util/query.hpp"
#include "io/map/emit/emit_info.hpp"
#include "io/map/emit/emitter.hpp"
#include "misc/assert.hpp"

// Reference: https://docs.godotengine.org/en/stable/development/file_formats/tscn.html

namespace tactile::io {
namespace {

using ExtResource = int32;
using SubResource = int32;
using Rect2 = Vector4i;

struct GodotTexture final
{
  UUID        tileset_uuid {};
  ExtResource id {};
};

struct GodotAnimationFrame final
{
  SubResource id {};
  Rect2       region {};
};

struct GodotAnimation final
{
  ExtResource                      texture_id {};
  std::string                      name;
  std::vector<GodotAnimationFrame> frames;
  float                            speed {1.0};
};

struct Identifiers final
{
  int32              next_ext_resource {1};
  int32              next_sub_resource {1};
  int32              ext_tileset_id {};
  Maybe<SubResource> sprite_frames_id;
};

struct GodotRectangleShape final
{
  SubResource id {};
  Vector2f    extents {};
};

struct TilesetExportInfo final
{
  std::vector<usize>     sorted_indices;
  HashMap<TileID, int32> tileset_id_for_first_tile;
};

struct GodotScene final
{
  GodotEmitOptions                       options;
  TilesetExportInfo                      export_info;
  Identifiers                            identifiers;
  std::vector<GodotTexture>              textures;
  HashMap<ObjectID, GodotRectangleShape> rectangle_shapes;
  HashMap<TileID, GodotAnimation>        animations;
};

[[nodiscard]] auto get_tileset_image_path(const ir::TilesetData&  tileset,
                                          const GodotEmitOptions& options) -> fs::path
{
  const auto filename = tileset.name + tileset.image_path.extension().string();
  return options.project_image_dir / filename;
}

void create_animation(GodotScene&             scene,
                      TileIndex               tile_index,
                      const ExtResource       texture_id,
                      const ir::TilesetData&  tileset,
                      const ir::MetaTileData& tile)
{
  const auto tile_id = tileset.first_tile + tile_index;

  TACTILE_ASSERT(!tile.frames.empty());
  TACTILE_ASSERT(!scene.animations.contains(tile_id));

  auto& animation = scene.animations[tile_id];
  animation.texture_id = texture_id;
  animation.name = fmt::format("Tile {}", tile_id);
  animation.speed = 1;

  // Godot only supports a single speed for animations, so we just use the first frame
  if (!tile.frames.empty()) {
    animation.speed = 1'000.0f / static_cast<float>(tile.frames.front().duration_ms);
  }

  for (const auto& frame : tile.frames) {
    const auto pos = TilePos::from_index(frame.local_id, tileset.column_count);

    auto& godot_frame = animation.frames.emplace_back();
    godot_frame.id = scene.identifiers.next_sub_resource++;
    godot_frame.region = Rect2 {pos.col_to_x(tileset.tile_size.x),
                                pos.row_to_y(tileset.tile_size.y),
                                tileset.tile_size.x,
                                tileset.tile_size.y};
  }
}

[[nodiscard]] auto get_tileset_export_info(const ir::MapData& map) -> TilesetExportInfo
{
  TilesetExportInfo info;

  std::vector<std::pair<usize, TileID>> pairs;
  pairs.reserve(map.tilesets.size());

  for (usize index = 0; const auto& tilesets : map.tilesets) {
    pairs.emplace_back(index, tilesets.first_tile);
    ++index;
  }

  std::sort(pairs.begin(), pairs.end(), [&](const auto& lhs, const auto& rhs) {
    return lhs.second < rhs.second;
  });

  info.sorted_indices.reserve(map.tilesets.size());
  for (const auto& [index, first_tile] : pairs) {
    info.sorted_indices.push_back(index);
    info.tileset_id_for_first_tile[first_tile] = static_cast<int32>(index) + 1;
  }

  return info;
}

[[nodiscard]] auto create_godot_scene_data(const ir::MapData& map,
                                           GodotEmitOptions   options) -> GodotScene
{
  GodotScene scene;
  scene.options = std::move(options);
  scene.identifiers.ext_tileset_id = scene.identifiers.next_ext_resource++;
  scene.export_info = get_tileset_export_info(map);

  for (const auto& tileset : map.tilesets) {
    auto& texture = scene.textures.emplace_back();
    texture.id = scene.identifiers.next_ext_resource++;
    texture.tileset_uuid = tileset.uuid;

    for (const auto& [tile_index, meta_tile] : tileset.fancy_tiles) {
      const auto tile_id = tileset.first_tile + tile_index;
      if (!meta_tile.frames.empty() && !scene.animations.contains(tile_id)) {
        create_animation(scene, tile_index, texture.id, tileset, meta_tile);
      }
    }

    if (!scene.animations.empty()) {
      scene.identifiers.sprite_frames_id = scene.identifiers.next_sub_resource++;
    }
  }

  for (const auto& layer : map.layers) {
    if (layer.type == LayerType::ObjectLayer) {
      const auto& object_layer = std::get<ir::ObjectLayerData>(layer.data);
      for (const auto& object : object_layer.objects) {
        if (object.type == ObjectType::Rect) {
          auto& shape = scene.rectangle_shapes[object.id];
          shape.id = scene.identifiers.next_sub_resource++;
          shape.extents = object.size / 2.0f;
        }
      }
    }
  }

  return scene;
}

void emit_tileset_file(const ir::MapData&       map,
                       const TilesetExportInfo& info,
                       const GodotEmitOptions&  options)
{
  const auto load_steps = map.tilesets.size() + 1;  // TODO validate

  const auto tileset_file =
      options.root_dir / options.project_tileset_dir / "tileset.tres";

  std::ofstream stream {tileset_file, std::ios::out | std::ios::trunc};
  stream << fmt::format(R"([gd_resource type="TileSet" load_steps={} format=2])",
                        load_steps)
         << '\n'
         << '\n';

  for (const auto tileset_index : info.sorted_indices) {
    const auto  id = static_cast<int32>(tileset_index) + 1;
    const auto& tileset = map.tilesets.at(tileset_index);

    const auto image_path = get_tileset_image_path(tileset, options);
    fs::copy(tileset.image_path,
             options.root_dir / image_path,
             fs::copy_options::overwrite_existing);

    stream << fmt::format(R"([ext_resource path="res://{}" type="Texture" id={}])",
                          convert_to_forward_slashes(image_path),
                          id)
           << '\n';
  }

  stream << "\n[resource]\n";

  for (const auto tileset_index : info.sorted_indices) {
    const auto& tileset = map.tilesets.at(tileset_index);

    const auto id = static_cast<int32>(tileset_index) + 1;
    const auto prefix = fmt::format("{}/", id);

    stream << prefix << fmt::format(R"(name = "{}")", tileset.name) << '\n';
    stream << prefix << fmt::format("texture = ExtResource( {} )\n", id);
    stream << prefix << "tex_offset = Vector2( 0, 0 )\n";
    stream << prefix << "modulate = Color( 1, 1, 1, 1 )\n";
    stream << prefix
           << fmt::format("region = Rect2( 0, 0, {}, {} )\n",
                          tileset.image_size.x,
                          tileset.image_size.y);
    stream << prefix << "tile_mode = 2\n";
    stream << prefix << "autotile/icon_coordinate = Vector2( 0, 0 )\n";
    stream << prefix
           << fmt::format("autotile/tile_size = Vector2( {}, {} )\n",
                          tileset.tile_size.x,
                          tileset.tile_size.y);
    stream << prefix << "autotile/spacing = 0\n";
    stream << prefix << "autotile/occluder_map = [  ]\n";
    stream << prefix << "autotile/navpoly_map = [  ]\n";
    stream << prefix << "autotile/priority_map = [  ]\n";
    stream << prefix << "autotile/z_index_map = [  ]\n";
    stream << prefix << "occluder_offset = Vector2( 0, 0 )\n";
    stream << prefix << "navigation_offset = Vector2( 0, 0 )\n";
    stream << prefix << "shape_offset = Vector2( 0, 0 )\n";
    stream << prefix << "shape_transform = Transform2D( 1, 0, 0, 1, 0, 0 )\n";
    stream << prefix << "shape_one_way = false\n";
    stream << prefix << "shape_one_way_margin = 0.0\n";
    stream << prefix << "shapes = [  ]\n";
    stream << prefix << "z_index = 0\n";
  }
}

void emit_textures(std::ostream&           stream,
                   const GodotScene&       scene,
                   const ir::MapData&      map,
                   const GodotEmitOptions& options)
{
  for (const auto& texture : scene.textures) {
    const auto& tileset = first_in(map.tilesets, [&](const ir::TilesetData& ts) {
      return ts.uuid == texture.tileset_uuid;
    });

    const auto path =
        convert_to_forward_slashes(get_tileset_image_path(tileset, options));
    stream << fmt::format(R"([ext_resource path="res://{}" type="Texture" id={}])",
                          path,
                          texture.id)
           << '\n';
  }
}

void emit_atlas_textures(std::ostream& stream, const GodotScene& scene)
{
  for (const auto& [tile_id, animation] : scene.animations) {
    for (const auto& frame : animation.frames) {
      stream << '\n';
      stream << fmt::format(R"([sub_resource type="AtlasTexture" id={}])", frame.id)
             << '\n';
      stream << fmt::format("atlas = ExtResource( {} )\n", animation.texture_id);
      stream << fmt::format("region = Rect2( {}, {}, {}, {} )\n",
                            frame.region.x,
                            frame.region.y,
                            frame.region.z,
                            frame.region.w);
    }
  }
}

void emit_sprite_frames(std::ostream& stream, const GodotScene& scene)
{
  const auto sprite_frames_id = scene.identifiers.sprite_frames_id;
  if (!sprite_frames_id.has_value()) {
    return;
  }

  stream << '\n';
  stream << fmt::format(R"([sub_resource type="SpriteFrames" id={}])", *sprite_frames_id)
         << '\n';
  stream << "animations = [\n";

  bool has_emitted_animation = false;
  for (const auto& [tile_id, animation] : scene.animations) {
    if (has_emitted_animation) {
      stream << ",\n {\n";
    }
    else {
      stream << " {\n";
    }

    stream << R"(   "frames": [)";
    bool has_emitted_frame = false;
    for (const auto& frame : animation.frames) {
      if (has_emitted_frame) {
        stream << ", ";
      }
      stream << fmt::format("SubResource( {} )", frame.id);
      has_emitted_frame = true;
    }
    stream << "],\n";
    stream << R"(   "loop": true,)" << '\n';
    stream << fmt::format(R"(   "name": "{}",)", animation.name) << '\n';
    stream << fmt::format(R"(   "speed": {})", animation.speed) << '\n';

    stream << " }";
    has_emitted_animation = true;
  }

  stream << "\n]\n";
}

void emit_shapes(std::ostream& stream, const GodotScene& scene)
{
  for (const auto& [object_id, shape] : scene.rectangle_shapes) {
    stream << fmt::format(R"([sub_resource type="RectangleShape2D" id={}])", shape.id)
           << '\n';
    stream << fmt::format("extents = Vector2( {}, {} )\n",
                          shape.extents.x,
                          shape.extents.y);
  }
}

void emit_tile_layer_animation_nodes(std::ostream&            stream,
                                     const GodotScene&        scene,
                                     const ir::MapData&       map,
                                     const ir::TileLayerData& tile_layer,
                                     std::string_view         layer_name)
{
  invoke_mn(tile_layer.row_count, tile_layer.col_count, [&](usize row, usize col) {
    const auto tile_id = tile_layer.tiles[row][col];
    if (tile_id != empty_tile && scene.animations.contains(tile_id)) {
      stream << '\n'
             << fmt::format(
                    R"-([node name="Tile ({}, {})" type="AnimatedSprite" parent="{}"])-",
                    row,
                    col,
                    layer_name)
             << '\n';

      const auto pos = TilePos::from(row, col);
      const auto x = pos.col_to_x(map.tile_size.x);
      const auto y = pos.row_to_y(map.tile_size.y);

      stream << fmt::format("position = Vector2( {}, {} )\n", x, y);
      stream << fmt::format("frames = SubResource( {} )\n",
                            scene.identifiers.sprite_frames_id.value());
      stream << "speed_scale = 1.0\n";
      stream << fmt::format(R"(animation = "Tile {}")", tile_id) << '\n';
      stream << "playing = true\n";
      stream << "centered = false\n";
    }
  });
}

void emit_tile_layer(std::ostream&        stream,
                     const GodotScene&    scene,
                     const ir::MapData&   map,
                     const ir::LayerData& layer,
                     std::string_view     parent)
{
  const auto& tile_layer = std::get<ir::TileLayerData>(layer.data);

  stream << '\n'
         << fmt::format(R"([node name="{}" type="TileMap" parent="{}"])",
                        layer.name,
                        parent)
         << '\n';
  stream << fmt::format("tile_set = ExtResource( {} )\n",
                        scene.identifiers.ext_tileset_id);
  stream << "format = 1\n";

  stream << "tile_data = PoolIntArray(";

  bool has_emitted_tile = false;

  invoke_mn(tile_layer.row_count, tile_layer.col_count, [&](usize row, usize col) {
    const auto tile_id = tile_layer.tiles[row][col];

    if (tile_id != empty_tile) {
      const auto x = static_cast<int32>(col);
      auto       y = static_cast<int32>(row);

      if (x < 0) {
        y += 1;
      }

      constexpr int32 tile_offset = 65'536;

      const auto& tileset = first_in(map.tilesets, [=](const ir::TilesetData& ts) {
        return tile_id >= ts.first_tile && tile_id <= ts.first_tile + ts.tile_count;
      });

      const auto parent_tileset =
          lookup_in(scene.export_info.tileset_id_for_first_tile, tileset.first_tile);

      const auto encoded_pos = x + (y * tile_offset);
      auto       encoded_tile = tile_id;

      const auto tile_index = tile_id - tileset.first_tile;

      if (tile_index >= tileset.column_count) {
        const auto tile_x = tile_index % tileset.column_count;
        const auto tile_y = tile_index / tileset.column_count;
        encoded_tile = tile_x + tile_y * tile_offset;
      }

      if (has_emitted_tile) {
        stream << ", ";
      }

      stream << fmt::format("{}, {}, {}", encoded_pos, parent_tileset, encoded_tile);
      has_emitted_tile = true;
    }
  });

  stream << ")\n";

  stream << fmt::format("cell_size = Vector2( {}, {} )\n",
                        map.tile_size.x,
                        map.tile_size.y);

  emit_tile_layer_animation_nodes(stream, scene, map, tile_layer, layer.name);
}

void emit_rectangle_object(std::ostream&         stream,
                           const GodotScene&     scene,
                           const ir::ObjectData& object,
                           std::string_view      object_name,
                           std::string_view      parent)
{
  stream << '\n'
         << fmt::format(R"([node name="{}" type="Area2D" parent="{}"])",
                        object_name,
                        parent)
         << '\n';
  stream << fmt::format("position = Vector2( {}, {} )\n",
                        object.pos.x + object.size.x / 2.0f,
                        object.pos.y + object.size.y / 2.0f);

  stream << '\n'
         << fmt::format(R"([node name="Shape" type="CollisionShape2D" parent="{}/{}"])",
                        parent,
                        object_name)
         << '\n';
  stream << fmt::format("shape = SubResource( {} )\n",
                        scene.rectangle_shapes.at(object.id).id);
}

[[nodiscard]] auto approximate_ellipse_as_polygon(const ir::ObjectData&   object,
                                                  const GodotEmitOptions& options)
    -> std::vector<Vector2f>
{
  TACTILE_ASSERT(object.type == ObjectType::Ellipse);

  std::vector<Vector2f> points;
  points.reserve(options.ellipse_polygon_point_count);

  const auto n = static_cast<double>(options.ellipse_polygon_point_count);
  const auto radius = object.size / 2.0f;

  constexpr auto tau = std::numbers::pi * 2.0;
  for (usize i = 0; i < options.ellipse_polygon_point_count; ++i) {
    const auto theta = static_cast<double>(i) / n * tau;
    const auto x = radius.x * std::cos(theta);
    const auto y = radius.y * std::sin(theta);
    points.emplace_back(x, y);
  }

  return points;
}

void emit_ellipse_object(std::ostream&         stream,
                         const GodotScene&     scene,
                         const ir::ObjectData& object,
                         std::string_view      object_name,
                         std::string_view      parent)
{
  stream << '\n'
         << fmt::format(R"([node name="{}" type="Area2D" parent="{}"])",
                        object_name,
                        parent)
         << '\n';
  stream << fmt::format("position = Vector2( {}, {} )\n",
                        object.pos.x + object.size.x / 2.0f,
                        object.pos.y + object.size.y / 2.0f);

  stream << '\n'
         << fmt::format(R"([node name="Shape" type="CollisionPolygon2D" parent="{}/{}"])",
                        parent,
                        object_name)
         << '\n';
  stream << "polygon = PoolVector2Array( ";

  const auto points = approximate_ellipse_as_polygon(object, scene.options);

  bool first = true;
  for (const auto& point : points) {
    if (!first) {
      stream << ", ";
    }
    stream << fmt::format("{:.3f}, {:.3f}", point.x, point.y);
    first = false;
  }

  stream << " )\n";
}

void emit_object(std::ostream&         stream,
                 const GodotScene&     scene,
                 const ir::ObjectData& object,
                 std::string_view      parent)
{
  auto object_name = fmt::format("Object {}", object.id);

  if (!object.name.empty()) {
    object_name += fmt::format(" ('{}')", object.name);
  }

  if (object.type == ObjectType::Rect) {
    emit_rectangle_object(stream, scene, object, object_name, parent);
  }
  else if (object.type == ObjectType::Ellipse) {
    emit_ellipse_object(stream, scene, object, object_name, parent);
  }
  else {
    stream << '\n'
           << fmt::format(R"([node name="{}" type="Node2D" parent="{}"])",
                          object_name,
                          parent)
           << '\n';
  }
}

void emit_object_layer(std::ostream&        stream,
                       const GodotScene&    scene,
                       const ir::LayerData& layer,
                       std::string_view     parent)
{
  stream << '\n'
         << fmt::format(R"([node name="{}" type="Node2D" parent="{}"])",
                        layer.name,
                        parent)
         << '\n';

  const auto& object_layer = std::get<ir::ObjectLayerData>(layer.data);
  for (const auto& object : object_layer.objects) {
    emit_object(stream, scene, object, layer.name);
  }
}

void emit_layer(std::ostream&        stream,
                const GodotScene&    scene,
                const ir::MapData&   map,
                const ir::LayerData& layer,
                std::string_view     parent)
{
  switch (layer.type) {
    case LayerType::TileLayer:
      emit_tile_layer(stream, scene, map, layer, parent);
      break;

    case LayerType::ObjectLayer:
      emit_object_layer(stream, scene, layer, parent);
      break;

    case LayerType::GroupLayer: {
      stream << '\n'
             << fmt::format(R"([node name="{}" type="Node2D" parent="{}"])",
                            layer.name,
                            parent)
             << '\n';

      const auto& group_layer = std::get<ir::GroupLayerData>(layer.data);
      const auto  parent_path =
          parent == "." ? layer.name : fmt::format("{}/{}", parent, layer.name);

      for (const auto& child_layer : group_layer.children) {
        emit_layer(stream, scene, map, *child_layer, parent_path);
      }

      break;
    }
  }
}

}  // namespace

// TODO sibling nodes cannot have the same names, validate!
void emit_godot_map(const EmitInfo& info, const GodotEmitOptions& options)
{
  const auto& map = info.data();

  const auto scene = create_godot_scene_data(map, options);
  emit_tileset_file(map, scene.export_info, scene.options);

  std::ofstream stream {info.destination_file(), std::ios::out | std::ios::trunc};

  const auto load_steps = map.tilesets.size() + scene.textures.size() + 1;  // FIXME
  stream << fmt::format("[gd_scene load_steps={} format=2]\n\n", load_steps);

  const auto tileset_file = options.project_tileset_dir / "tileset.tres";
  stream << fmt::format(R"([ext_resource path="res://{}" type="TileSet" id={}])",
                        convert_to_forward_slashes(tileset_file),
                        scene.identifiers.ext_tileset_id)
         << '\n';

  emit_textures(stream, scene, map, options);
  emit_atlas_textures(stream, scene);
  emit_sprite_frames(stream, scene);
  emit_shapes(stream, scene);

  // This is the root node
  stream << '\n'
         << fmt::format(R"([node name="{}" type="Node2D"])",
                        info.destination_file().stem().string())
         << '\n';

  for (const auto& layer : map.layers) {
    emit_layer(stream, scene, map, layer, ".");
  }
}

}  // namespace tactile::io