// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/godot_tscn/gd3_exporter.hpp"

#include <cstddef>  // size_t
#include <format>   // format
#include <fstream>  // ofstream
#include <iomanip>  // quoted, setprecision
#include <ios>      // boolalpha, fixed
#include <ostream>  // ostream
#include <sstream>  // stringstream

#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/godot_tscn/gd3_scene_writer.hpp"
#include "tactile/godot_tscn/gd3_types.hpp"
#include "tactile/godot_tscn/logging.hpp"

namespace tactile::godot_tscn {
namespace {

void _emit_attribute(std::ostream& stream, const std::string_view name, const Attribute& value)
{
  stream << std::quoted(name) << ": ";

  switch (value.get_type()) {
    case AttributeType::kStr: {
      stream << std::quoted(value.as_string());
      break;
    }
    case AttributeType::kInt: {
      stream << value.as_int();
      break;
    }
    case AttributeType::kInt2: {
      const auto& vec = value.as_int2();
      stream << "Vector2( " << vec.x() << ", " << vec.y() << " )";
      break;
    }
    case AttributeType::kInt3: {
      const auto& vec = value.as_int3();
      stream << "Vector3( " << vec.x() << ", " << vec.y() << ", " << vec.z() << " )";
      break;
    }
    case AttributeType::kInt4: {
      const auto& vec = value.as_int4();
      stream << "[ " << vec.x() << ", " << vec.y() << ", " << vec.z() << ", " << vec.w()
             << " ]";
      break;
    }
    case AttributeType::kFloat: {
      stream << value.as_float();
      break;
    }
    case AttributeType::kFloat2: {
      const auto& vec = value.as_float2();
      stream << "Vector2( " << vec.x() << ", " << vec.y() << " )";
      break;
    }
    case AttributeType::kFloat3: {
      const auto& vec = value.as_float3();
      stream << "Vector3( " << vec.x() << ", " << vec.y() << ", " << vec.z() << " )";
      break;
    }
    case AttributeType::kFloat4: {
      const auto& vec = value.as_float4();
      stream << "[ " << vec.x() << ", " << vec.y() << ", " << vec.z() << ", " << vec.w()
             << " ]";
      break;
    }
    case AttributeType::kBool: {
      stream << value.as_bool();
      break;
    }
    case AttributeType::kPath: {
      const auto& path_string = value.as_path().string();
      stream << std::quoted(path_string);
      break;
    }
    case AttributeType::kColor: {
      // TODO use normalize function
      const auto& color = value.as_color();

      const auto r = static_cast<float>(color.red) / 255.0f;
      const auto g = static_cast<float>(color.green) / 255.0f;
      const auto b = static_cast<float>(color.blue) / 255.0f;
      const auto a = static_cast<float>(color.alpha) / 255.0f;

      stream << "Color( " << r << ", " << g << ", " << b << ", " << a << " )";
      break;
    }
    case AttributeType::kObject: {
      stream << value.as_object().value;
      break;
    }
    default: throw std::runtime_error {"bad attribute"};
  }
}

void _emit_attributes(std::ostream& stream, const StringMap<Attribute>& attributes)
{
  std::size_t index = 0;
  for (const auto& [name, value] : attributes) {
    if (index != 0) {
      stream << ',';
    }

    _emit_attribute(stream, name, value);
    ++index;
  }
}

void _emit_components(std::ostream& stream, const StringMap<StringMap<Attribute>>& components)
{
  std::size_t component_index = 0;
  for (const auto& [comp_name, comp_attributes] : components) {
    if (component_index != 0) {
      stream << ", ";
    }

    stream << std::quoted(comp_name) << ": {";
    _emit_attributes(stream, comp_attributes);
    stream << '}';

    ++component_index;
  }
}

void _emit_metadata(Gd3SceneWriter& writer, const Gd3Metadata& meta)
{
  const auto has_props = !meta.props.empty();
  const auto has_comps = !meta.comps.empty();

  if (!has_props && !has_comps) {
    return;
  }

  std::stringstream meta_stream {};
  meta_stream << std::setprecision(3) << std::fixed << std::boolalpha;

  meta_stream << '{';

  if (has_props) {
    meta_stream << "\"properties\": {";
    _emit_attributes(meta_stream, meta.props);
    meta_stream << '}';
  }

  if (has_comps) {
    meta_stream << "\"components\": {";
    _emit_components(meta_stream, meta.comps);
    meta_stream << '}';
  }

  meta_stream << '}';

  const auto meta_string = meta_stream.str();
  writer.variable("__meta__", meta_string);
}

void _emit_tile_atlas(Gd3SceneWriter& writer,
                      const Gd3TileAtlas& tile_atlas,
                      const std::size_t index)
{
  writer.set_key_prefix(std::format("{}/", index));

  writer.variable_quoted("name", tile_atlas.name)
      .ext_resource_variable("texture", tile_atlas.texture_id)
      .variable("tex_offset", "Vector2( 0, 0 )")
      .variable("modulate", "Color( 1, 1, 1, 1 )")
      .rect2_variable("region",
                      Int4 {0, 0, tile_atlas.image_size.x(), tile_atlas.image_size.y()})
      .variable("tile_mode", "2")
      .variable("occluder_offset", "Vector2( 0, 0 )")
      .variable("navigation_offset", "Vector2( 0, 0 )")
      .variable("shape_offset", "Vector2( 0, 0 )")
      .variable("shape_transform", "Transform2D( 1, 0, 0, 1, 0, 0 )")
      .variable("shape_one_way", "false")
      .variable("shape_one_way_margin", "0.0")
      .variable("shapes", "[  ]")
      .variable("z_index", "0");

  writer.set_key_prefix(std::format("{}/autotile/", index));

  writer.variable("icon_coordinate", "Vector2( 0, 0 )")
      .vector2_variable("tile_size", tile_atlas.tile_size)
      .variable("spacing", "0")
      .variable("occluder_map", "[  ]")
      .variable("navpoly_map", "[  ]")
      .variable("priority_map", "[  ]")
      .variable("z_index_map", "[  ]");

  writer.set_key_prefix("");
}

void _emit_atlas_texture(Gd3SceneWriter& writer,
                         const SubResourceId id,
                         const Gd3AtlasTexture& texture)
{
  writer.newline()
      .sub_resource_header("AtlasTexture", id)
      .ext_resource_variable("atlas", texture.atlas_id)
      .rect2_variable("region", texture.region);
}

void _emit_sprite_frames(Gd3SceneWriter& writer, const Gd3SpriteFrames& sprite_frames)
{
  if (sprite_frames.animations.empty()) {
    return;
  }

  writer.newline().sub_resource_header("SpriteFrames", sprite_frames.id);

  std::stringstream animations_stream {};
  animations_stream << std::setprecision(3) << std::fixed;
  animations_stream << '[';

  for (std::size_t anim_index = 0; const auto& animation : sprite_frames.animations) {
    if (anim_index != 0) {
      animations_stream << ", ";
    }
    animations_stream << '{';

    animations_stream << "\"loop\": true";
    animations_stream << ", \"name\": \"" << animation.name << "\"";
    animations_stream << ", \"speed\": \"" << animation.speed << "\"";
    animations_stream << ", \"frames\": [";

    for (std::size_t frame_index = 0; const auto atlas_texture_id : animation.frames) {
      if (frame_index != 0) {
        animations_stream << ", ";
      }
      animations_stream << "SubResource( " << atlas_texture_id << " )";
      ++frame_index;
    }

    animations_stream << "]}";
    ++anim_index;
  }

  animations_stream << ']';

  const auto animations_string = animations_stream.str();
  writer.variable("animations", animations_string);
}

void _emit_tile_layer_animation_nodes(Gd3SceneWriter& writer,
                                      const Gd3TileLayer& tile_layer,
                                      const SubResourceId sprite_frames_id)
{
  for (const auto& animation : tile_layer.animations) {
    const auto name = std::format("Tile {}", animation.position);
    const auto animation_name = std::format("Tile {}", animation.tile_id);

    writer.newline()
        .node_header(name, "AnimatedSprite", animation.parent)
        .vector2_variable("position", to_int2(animation.position) * tile_layer.cell_size)
        .sub_resource_variable("frames", sprite_frames_id)
        .variable("speed_scale", "1.0")
        .variable_quoted("animation", animation_name)
        .variable("playing", "true")
        .variable("centered", "false");
  }
}

void _emit_tile_layer(Gd3SceneWriter& writer,
                      const Gd3Layer& layer,
                      const SubResourceId tileset_id,
                      const SubResourceId sprite_frames_id)
{
  const auto& tile_layer = std::get<Gd3TileLayer>(layer.value);

  writer.newline()
      .node_header(layer.name, "TileMap", layer.parent)
      .sub_resource_variable("tile_set", tileset_id)
      .variable("visible", layer.visible)
      .vector2_variable("cell_size", tile_layer.cell_size)
      .color_variable("modulate", FColor {1, 1, 1, layer.opacity})
      .variable("format", "1")
      .sequence_variable("tile_data",
                         "PoolIntArray",
                         tile_layer.tiles,
                         [](std::ostream& stream, const Gd3EncodedTile& tile) {
                           stream << tile.position << ", " << tile.tile_atlas << ", "
                                  << tile.tile_index;
                         });

  _emit_metadata(writer, layer.meta);

  _emit_tile_layer_animation_nodes(writer, tile_layer, sprite_frames_id);
}

void _emit_rect_object(Gd3SceneWriter& writer, const Gd3Object& object)
{
  const auto& rect = std::get<Gd3Rect>(object.value);

  writer.newline()
      .node_header(object.name, "Area2D", object.parent)
      .vector2_variable("position", object.position)
      .variable("visible", object.visible);

  _emit_metadata(writer, object.meta);

  const auto shape_parent_path = std::format("{}/{}", object.parent, object.name);
  writer.newline()
      .node_header("Shape", "CollisionShape2D", shape_parent_path)
      .sub_resource_variable("shape", rect.shape_id);
}

void _emit_polygon_object(Gd3SceneWriter& writer, const Gd3Object& object)
{
  const auto& polygon = std::get<Gd3Polygon>(object.value);

  writer.newline()
      .node_header(object.name, "Area2D", object.parent)
      .vector2_variable("position", object.position)
      .variable("visible", object.visible);

  _emit_metadata(writer, object.meta);

  const auto shape_parent = std::format("{}/{}", object.parent, object.name);

  writer.newline()
      .node_header("Shape", "CollisionPolygon2D", shape_parent)
      .sequence_variable("polygon",
                         "PoolVector2Array",
                         polygon.points,
                         [](std::ostream& stream, const Float2& point) {
                           stream << point.x() << ", " << point.y();
                         });
}

void _emit_point_object(Gd3SceneWriter& writer, const Gd3Object& object)
{
  writer.newline()
      .node_header(object.name, "Node2D", object.parent)
      .vector2_variable("position", object.position)
      .variable("visible", object.visible);

  _emit_metadata(writer, object.meta);
}

void _emit_object(Gd3SceneWriter& writer, const Gd3Object& object)
{
  switch (object.value.index()) {
    case Gd3Object::kRectTypeIndex:    _emit_rect_object(writer, object); break;
    case Gd3Object::kPolygonTypeIndex: _emit_polygon_object(writer, object); break;
    case Gd3Object::kPointTypeIndex:   _emit_point_object(writer, object); break;
    default:                           throw std::runtime_error {"bad object"};
  }
}

void _emit_object_layer(Gd3SceneWriter& writer, const Gd3Layer& layer)
{
  const auto& object_layer = std::get<Gd3ObjectLayer>(layer.value);

  writer.newline()
      .node_header(layer.name, "Node2D", layer.parent)
      .color_variable("modulate", FColor {1, 1, 1, layer.opacity})
      .variable("visible", layer.visible);

  _emit_metadata(writer, layer.meta);

  for (const auto& object : object_layer.objects) {
    _emit_object(writer, object);
  }
}

void _emit_layer(Gd3SceneWriter& writer,
                 const Gd3Layer& layer,
                 SubResourceId tileset_id,
                 SubResourceId sprite_frames_id);

void _emit_group_layer(Gd3SceneWriter& writer,
                       const Gd3Layer& layer,
                       const SubResourceId tileset_id,
                       const SubResourceId sprite_frames_id)
{
  const auto& group_layer = std::get<Gd3GroupLayer>(layer.value);

  writer.newline()
      .node_header(layer.name, "Node2D", layer.parent)  //
      .color_variable("modulate", FColor {1, 1, 1, layer.opacity})
      .variable("visible", layer.visible);

  _emit_metadata(writer, layer.meta);

  for (const auto& sublayer : group_layer.layers) {
    _emit_layer(writer, sublayer, tileset_id, sprite_frames_id);
  }
}

void _emit_layer(Gd3SceneWriter& writer,
                 const Gd3Layer& layer,
                 const SubResourceId tileset_id,
                 const SubResourceId sprite_frames_id)
{
  switch (layer.value.index()) {
    case Gd3Layer::kTileLayerTypeIndex: {
      _emit_tile_layer(writer, layer, tileset_id, sprite_frames_id);
      break;
    }
    case Gd3Layer::kObjectLayerTypeIndex: {
      _emit_object_layer(writer, layer);
      break;
    }
    case Gd3Layer::kGroupLayerTypeIndex: {
      _emit_group_layer(writer, layer, tileset_id, sprite_frames_id);
      break;
    }
    default: throw std::runtime_error {"bad layer"};
  }
}

void _emit_tileset(Gd3SceneWriter& writer, const Gd3Tileset& tileset)
{
  writer.newline().sub_resource_header("TileSet", tileset.id);

  _emit_metadata(writer, tileset.meta);

  std::size_t tileset_index = 0;
  for (const auto& tile_atlas : tileset.atlases) {
    _emit_tile_atlas(writer, tile_atlas, tileset_index);
    ++tileset_index;
  }
}

void _emit_resources(Gd3SceneWriter& writer, const Gd3Resources& resources)
{
  for (const auto& [id, resource] : resources.ext_resources) {
    writer.newline().ext_resource_header(id, resource);
  }
}

[[nodiscard]]
auto _emit_map_file(const Gd3Map& map, const SaveFormatWriteOptions& options)
    -> std::expected<void, ErrorCode>
{
  const auto path = options.base_dir / "map.tscn";
  TACTILE_GODOT_TSCN_DEBUG("Generating map scene '{}'", path.string());

  std::ofstream stream {path, std::ios::out | std::ios::trunc};
  if (!stream.good()) {
    return std::unexpected {ErrorCode::kBadFileStream};
  }

  Gd3SceneWriter writer {stream};

  const auto load_steps = map.resources.ext_resources.size() +
                          saturate_cast<std::size_t>(map.resources.next_sub_resource_id - 1);
  writer.gd_scene_header(load_steps);

  _emit_resources(writer, map.resources);

  for (const auto& [id, texture] : map.atlas_textures) {
    _emit_atlas_texture(writer, id, texture);
  }

  for (const auto& [id, shape] : map.rect_shapes) {
    writer.newline()
        .sub_resource_header("RectangleShape2D", id)
        .vector2_variable("extents", shape.extents);
  }

  _emit_sprite_frames(writer, map.sprite_frames);
  _emit_tileset(writer, map.tileset);

  writer.newline().node_header("Root", "Node2D");
  _emit_metadata(writer, map.meta);

  for (const auto& layer : map.layers) {
    _emit_layer(writer, layer, map.tileset.id, map.sprite_frames.id);
  }

  return {};
}

[[nodiscard]]
auto _save_tileset_images(const Gd3Tileset& tileset, const SaveFormatWriteOptions& options)
    -> std::expected<void, ErrorCode>
{
  for (const auto& tile_atlas : tileset.atlases) {
    const auto dest = options.base_dir / tile_atlas.image_path.filename();  // FIXME

    TACTILE_GODOT_TSCN_DEBUG("Copying texture '{}' to '{}'",
                             tile_atlas.image_path.filename().string(),
                             dest.string());

    std::error_code copy_error {};
    std::filesystem::copy(tile_atlas.image_path,
                          dest,
                          std::filesystem::copy_options::overwrite_existing,
                          copy_error);

    if (copy_error) {
      return std::unexpected {ErrorCode::kBadFileCopy};
    }
  }

  return {};
}

}  // namespace

auto save_godot3_scene(const Gd3Map& map, const SaveFormatWriteOptions& options)
    -> std::expected<void, ErrorCode>
{
  return _save_tileset_images(map.tileset, options).and_then([&] {
    return _emit_map_file(map, options);
  });
}

}  // namespace tactile::godot_tscn
