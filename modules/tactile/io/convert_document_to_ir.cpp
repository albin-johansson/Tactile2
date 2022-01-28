#include "convert_document_to_ir.hpp"

#include <filesystem>  // absolute

#include "core/components/animation.hpp"
#include "core/components/attribute_context.hpp"
#include "core/components/component.hpp"
#include "core/components/fancy_tile.hpp"
#include "core/components/layer.hpp"
#include "core/components/object.hpp"
#include "core/components/parent.hpp"
#include "core/components/property.hpp"
#include "core/components/texture.hpp"
#include "core/components/tileset.hpp"
#include "core/map.hpp"
#include "core/systems/component_system.hpp"
#include "core/systems/tileset_system.hpp"
#include "editor/document.hpp"
#include "profile.hpp"
#include "tactile_def.hpp"

namespace tactile {
namespace {

[[nodiscard]] auto ConvertColor(const cen::color& color) -> IO::Color
{
  return {color.red(), color.green(), color.blue(), color.alpha()};
}

template <typename T>
void ConvertProperties(T& source,
                       const entt::registry& registry,
                       const std::vector<entt::entity>& properties)

{
  IO::ReserveProperties(source, properties.size());

  for (const auto propertyEntity : properties) {
    const auto& property = registry.get<comp::property>(propertyEntity);

    auto& data = IO::AddProperty(source);
    IO::SetName(data, property.name.c_str());

    switch (property.value.type()) {
      case attribute_type::string:
        IO::AssignString(data, property.value.as_string().c_str());
        break;

      case attribute_type::integer:
        IO::AssignInt(data, property.value.as_int());
        break;

      case attribute_type::floating:
        IO::AssignFloat(data, property.value.as_float());
        break;

      case attribute_type::boolean:
        IO::AssignBool(data, property.value.as_bool());
        break;

      case attribute_type::color: {
        IO::AssignColor(data, ConvertColor(property.value.as_color()));
        break;
      }

      case attribute_type::file:
        IO::AssignFile(data, property.value.as_file().c_str());
        break;

      case attribute_type::object:
        IO::AssignObject(data, property.value.as_object());
        break;
    }
  }
}

template <typename T>
void ConvertComponents(const IO::Map& ir,
                       T& source,
                       const entt::registry& registry,
                       const std::vector<entt::entity>& components)
{
  IO::ReserveComponents(source, components.size());

  for (const auto componentEntity : components) {
    const auto& component = registry.get<comp::component>(componentEntity);

    const auto& type = sys::get_component_def_name(registry, component.type);
    const auto& irDef = IO::GetComponentDef(ir, type.c_str());

    auto& irComponent = IO::AddComponent(source, irDef);

    for (const auto& [attributeName, attribute] : component.values) {
      switch (attribute.type()) {
        case attribute_type::string:
          IO::SetAttributeString(irComponent,
                                 attributeName.c_str(),
                                 attribute.as_string().c_str());
          break;

        case attribute_type::integer:
          IO::SetAttributeInt(irComponent, attributeName.c_str(), attribute.as_int());
          break;

        case attribute_type::floating:
          IO::SetAttributeFloat(irComponent, attributeName.c_str(), attribute.as_float());
          break;

        case attribute_type::boolean:
          IO::SetAttributeBool(irComponent, attributeName.c_str(), attribute.as_bool());
          break;

        case attribute_type::color: {
          IO::SetAttributeColor(irComponent,
                                attributeName.c_str(),
                                ConvertColor(attribute.as_color()));
          break;
        }

        case attribute_type::file:
          IO::SetAttributeFile(irComponent,
                               attributeName.c_str(),
                               attribute.as_file().c_str());
          break;

        case attribute_type::object:
          IO::SetAttributeObject(irComponent,
                                 attributeName.c_str(),
                                 attribute.as_object());
          break;
      }
    }
  }
}

void ConvertObject(IO::Map& irMap,
                   IO::Object& ir,
                   const entt::registry& registry,
                   const entt::entity entity)
{
  const auto& object = registry.get<comp::object>(entity);
  IO::SetId(ir, object.id);
  IO::SetX(ir, object.x);
  IO::SetY(ir, object.y);
  IO::SetWidth(ir, object.width);
  IO::SetHeight(ir, object.height);
  IO::SetType(ir, object.type);
  IO::SetTag(ir, object.tag.c_str());
  IO::SetVisible(ir, object.visible);

  const auto& context = registry.get<comp::attribute_context>(entity);
  IO::SetName(ir, context.name.c_str());
  ConvertProperties(ir, registry, context.properties);
  ConvertComponents(irMap, ir, registry, context.components);
}

void ConvertFancyTiles(IO::Map& irMap,
                       IO::Tileset& ir,
                       const entt::registry& registry,
                       const comp::tileset& tileset)

{
  for (auto&& [entity, tile, ctx] :
       registry.view<comp::fancy_tile, comp::attribute_context>().each()) {
    if (tile.id >= tileset.first_id && tile.id <= tileset.last_id) {
      auto& tileData = IO::AddTile(ir);
      IO::SetId(tileData, sys::convert_to_local(registry, tile.id).value());

      if (const auto* animation = registry.try_get<comp::animation>(entity)) {
        IO::ReserveAnimationFrames(tileData, animation->frames.size());

        for (const auto frameEntity : animation->frames) {
          const auto& frame = registry.get<comp::animation_frame>(frameEntity);

          auto& frameData = IO::AddAnimationFrame(tileData);
          IO::SetTile(frameData, sys::convert_to_local(registry, frame.tile).value());
          IO::SetDuration(frameData, static_cast<int32>(frame.duration.count()));
        }
      }

      IO::ReserveObjects(tileData, tile.objects.size());
      for (const auto objectEntity : tile.objects) {
        auto& objectData = IO::AddObject(tileData);
        ConvertObject(irMap, objectData, registry, objectEntity);
      }

      ConvertProperties(tileData, registry, ctx.properties);
      ConvertComponents(irMap, tileData, registry, ctx.components);
    }
  }
}

void ConvertTileset(IO::Map& irMap,
                    IO::Tileset& ir,
                    const entt::registry& registry,
                    const entt::entity entity,
                    const comp::tileset& tileset)
{
  IO::SetFirstGlobalId(ir, tileset.first_id);
  IO::SetTileWidth(ir, tileset.tile_width);
  IO::SetTileHeight(ir, tileset.tile_height);
  IO::SetTileCount(ir, tileset.tile_count);
  IO::SetColumnCount(ir, tileset.column_count);

  const auto& texture = registry.get<comp::texture>(entity);
  IO::SetImagePath(ir, std::filesystem::absolute(texture.path).c_str());
  IO::SetImageWidth(ir, texture.width);
  IO::SetImageHeight(ir, texture.height);

  const auto& ctx = registry.get<comp::attribute_context>(entity);
  IO::SetName(ir, ctx.name.c_str());

  ConvertProperties(ir, registry, ctx.properties);
  ConvertComponents(irMap, ir, registry, ctx.components);
  ConvertFancyTiles(irMap, ir, registry, tileset);
}

void ConvertLayer(IO::Map& irMap,
                  IO::Layer& ir,
                  const entt::registry& registry,
                  const entt::entity entity,
                  const comp::layer_tree_node& node,
                  const usize nRows,
                  const usize nCols)
{
  const auto& layer = registry.get<comp::layer>(entity);
  const auto& context = registry.get<comp::attribute_context>(entity);

  IO::SetIndex(ir, node.index);
  IO::SetId(ir, layer.id);
  IO::SetOpacity(ir, layer.opacity);
  IO::SetVisible(ir, layer.visible);

  IO::SetName(ir, context.name.c_str());
  ConvertProperties(ir, registry, context.properties);
  ConvertComponents(irMap, ir, registry, context.components);

  switch (layer.type) {
    case LayerType::TileLayer: {
      auto& tileLayer = IO::MarkAsTileLayer(ir);
      IO::ReserveTiles(tileLayer, nRows, nCols);

      const auto matrix = registry.get<comp::tile_layer>(entity).matrix;
      for (usize row = 0; row < nRows; ++row) {
        for (usize col = 0; col < nCols; ++col) {
          IO::SetTile(tileLayer, row, col, matrix.at(row).at(col));
        }
      }

      break;
    }

    case LayerType::ObjectLayer: {
      const auto& objectLayer = registry.get<comp::object_layer>(entity);

      auto& objectLayerData = IO::MarkAsObjectLayer(ir);
      IO::ReserveObjects(objectLayerData, objectLayer.objects.size());

      for (const auto objectEntity : objectLayer.objects) {
        auto& object = IO::AddObject(objectLayerData);
        ConvertObject(irMap, object, registry, objectEntity);
      }

      break;
    }

    case LayerType::GroupLayer: {
      auto& groupLayer = IO::MarkAsGroupLayer(ir);
      IO::ReserveLayers(groupLayer, node.children.size());

      for (const auto child : node.children) {
        auto& childData = IO::AddLayer(groupLayer);
        const auto& childNode = registry.get<comp::layer_tree_node>(child);
        ConvertLayer(irMap, childData, registry, child, childNode, nRows, nCols);
      }

      break;
    }
  }
}

void ConvertLayers(IO::Map& irMap, const entt::registry& registry)
{
  const auto& map = registry.ctx<MapInfo>();

  IO::ReserveLayers(irMap, registry.storage<comp::layer_tree_node>().size());
  for (auto&& [entity, node] : registry.view<comp::layer_tree_node>().each()) {
    const auto& parent = registry.get<comp::parent>(entity);
    if (parent.entity == entt::null) {
      auto& layerData = IO::AddLayer(irMap);
      ConvertLayer(irMap,
                   layerData,
                   registry,
                   entity,
                   node,
                   map.row_count,
                   map.column_count);
    }
  }
}

void ConvertTilesets(IO::Map& irMap, const entt::registry& registry)
{
  IO::ReserveTilesets(irMap, registry.storage<comp::tileset>().size());
  for (auto&& [entity, tileset] : registry.view<comp::tileset>().each()) {
    auto& tilesetData = IO::AddTileset(irMap);
    ConvertTileset(irMap, tilesetData, registry, entity, tileset);
  }
}

void ConvertMapAttributes(IO::Map& ir, const Document& document)
{
  const auto& map = document.registry.ctx<MapInfo>();
  IO::SetPath(ir, std::filesystem::absolute(document.path).c_str());

  IO::SetNextLayerId(ir, map.next_layer_id);
  IO::SetNextObjectId(ir, map.next_object_id);

  IO::SetTileWidth(ir, map.tile_width);
  IO::SetTileHeight(ir, map.tile_height);

  IO::SetRowCount(ir, map.row_count);
  IO::SetColumnCount(ir, map.column_count);
}

void ConvertComponentDefinitions(IO::Map& ir, const entt::registry& registry)
{
  for (auto&& [entity, def] : registry.view<comp::component_def>().each()) {
    auto& definition = IO::DefineComponent(ir, def.name.c_str());

    for (const auto& [name, value] : def.attributes) {
      const auto type = value.type();
      IO::DefineAttribute(definition, name.c_str(), type);

      switch (type) {
        case attribute_type::string:
          IO::SetAttributeDefaultString(definition,
                                        name.c_str(),
                                        value.as_string().c_str());
          break;

        case attribute_type::integer:
          IO::SetAttributeDefaultInt(definition, name.c_str(), value.as_int());
          break;

        case attribute_type::floating:
          IO::SetAttributeDefaultFloat(definition, name.c_str(), value.as_float());
          break;

        case attribute_type::boolean:
          IO::SetAttributeDefaultBool(definition, name.c_str(), value.as_bool());
          break;

        case attribute_type::color:
          IO::SetAttributeDefaultColor(definition,
                                       name.c_str(),
                                       ConvertColor(value.as_color()));
          break;

        case attribute_type::file:
          IO::SetAttributeDefaultFile(definition, name.c_str(), value.as_file().c_str());
          break;

        case attribute_type::object:
          IO::SetAttributeDefaultObject(definition, name.c_str(), value.as_object());
          break;
      }
    }
  }
}

}  // namespace

auto ConvertDocumentToIR(const Document& document) -> IO::MapPtr
{
  TACTILE_PROFILE_START;

  const auto& registry = document.registry;

  auto irMap = IO::CreateMap();

  ConvertComponentDefinitions(*irMap, registry);
  ConvertMapAttributes(*irMap, document);

  ConvertTilesets(*irMap, registry);
  ConvertLayers(*irMap, registry);

  const auto& context = registry.ctx<comp::attribute_context>();
  ConvertProperties(*irMap, registry, context.properties);
  ConvertComponents(*irMap, *irMap, registry, context.components);

  TACTILE_PROFILE_END("Converted document to intermediate representation");
  return irMap;
}

}  // namespace tactile
