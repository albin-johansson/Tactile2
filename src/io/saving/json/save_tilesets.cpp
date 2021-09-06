#include "save_tilesets.hpp"

#include <format>   // format
#include <utility>  // move

#include "core/components/animation.hpp"
#include "core/components/fancy_tile.hpp"
#include "core/components/property_context.hpp"
#include "core/components/texture.hpp"
#include "core/components/tileset.hpp"
#include "core/systems/tileset_system.hpp"
#include "io/preferences.hpp"
#include "io/saving/common_saving.hpp"
#include "save_json.hpp"
#include "save_properties.hpp"

namespace Tactile::IO {
namespace {

[[nodiscard]] auto SaveFancyTiles(const entt::registry& registry,
                                  const entt::entity tilesetEntity,
                                  const Tileset& tileset,
                                  const std::filesystem::path& dir) -> JSON
{
  auto array = JSON::array();

  for (auto&& [entity, tile] : registry.view<FancyTile>().each())
  {
    auto& tileJson = array.emplace_back();
    tileJson["id"] = Sys::ConvertToLocal(registry, tile.id).value().get();

    if (const auto* animation = registry.try_get<Animation>(entity))
    {
      auto animationJson = JSON::array();
      for (const auto frameEntity : animation->frames)
      {
        const auto& frame = registry.get<AnimationFrame>(frameEntity);
        auto frameJson = JSON::object();

        frameJson["tileid"] =
            Sys::ConvertToLocal(registry, frame.tile).value().get();
        frameJson["duration"] = frame.duration.count();

        animationJson += std::move(frameJson);
      }

      tileJson["animation"] = std::move(animationJson);
    }

    const auto& ctx = registry.get<PropertyContext>(entity);
    if (!ctx.properties.empty())
    {
      tileJson["properties"] = SaveProperties(registry, entity, dir);
    }
  }

  return array;
}

void AddCommonAttributes(JSON& json,
                         const entt::registry& registry,
                         const entt::entity tilesetEntity,
                         const Tileset& tileset,
                         const std::filesystem::path& dir)
{
  const auto& context = registry.get<PropertyContext>(tilesetEntity);
  const auto& texture = registry.get<Texture>(tilesetEntity);

  json["name"] = context.name;
  json["columns"] = tileset.column_count.get();
  json["image"] = GetTilesetImagePath(tileset, dir);
  json["imagewidth"] = texture.width;
  json["imageheight"] = texture.height;
  json["margin"] = 0;
  json["spacing"] = 0;
  json["tilecount"] = tileset.tile_count;
  json["tilewidth"] = tileset.tile_width;
  json["tileheight"] = tileset.tile_height;
  json["tiles"] = SaveFancyTiles(registry, tilesetEntity, tileset, dir);

  const auto& ctx = registry.get<PropertyContext>(tilesetEntity);
  if (!ctx.properties.empty())
  {
    json["properties"] = SaveProperties(registry, tilesetEntity, dir);
  }
}

[[nodiscard]] auto SaveEmbeddedTileset(const entt::registry& registry,
                                       const entt::entity tilesetEntity,
                                       const Tileset& tileset,
                                       const std::filesystem::path& dir) -> JSON
{
  auto json = JSON::object();

  json["firstgid"] = tileset.first_id.get();
  AddCommonAttributes(json, registry, tilesetEntity, tileset, dir);

  return json;
}

[[nodiscard]] auto SaveExternalTileset(const entt::registry& registry,
                                       const entt::entity tilesetEntity,
                                       const Tileset& tileset) -> JSON
{
  const auto& context = registry.get<PropertyContext>(tilesetEntity);
  auto json = JSON::object();

  json["firstgid"] = tileset.first_id.get();
  json["source"] = std::format("{}.json", context.name);

  return json;
}

void CreateExternalTilesetFile(const entt::registry& registry,
                               const entt::entity tilesetEntity,
                               const Tileset& tileset,
                               const std::filesystem::path& dir)
{
  auto json = JSON::object();

  AddCommonAttributes(json, registry, tilesetEntity, tileset, dir);

  json["type"] = "tileset";
  json["tiledversion"] = tiled_version;
  json["version"] = tiled_json_version;

  const auto& context = registry.get<PropertyContext>(tilesetEntity);
  const auto name = std::format("{}.json", context.name);
  const auto path = dir / name;

  CENTURION_LOG_INFO("Saving external tileset in \"%s\"", path.string().c_str());
  SaveJson(json, path);
}

[[nodiscard]] auto SaveTileset(const entt::registry& registry,
                               const entt::entity tilesetEntity,
                               const Tileset& tileset,
                               const std::filesystem::path& dir) -> JSON
{
  if (Prefs::GetEmbedTilesets())
  {
    return SaveEmbeddedTileset(registry, tilesetEntity, tileset, dir);
  }
  else
  {
    CreateExternalTilesetFile(registry, tilesetEntity, tileset, dir);
    return SaveExternalTileset(registry, tilesetEntity, tileset);
  }
}

}  // namespace

[[nodiscard]] auto SaveTilesets(const entt::registry& registry,
                                const std::filesystem::path& dir) -> JSON
{
  auto json = JSON::array();

  for (auto&& [entity, tileset] : registry.view<Tileset>().each())
  {
    json += SaveTileset(registry, entity, tileset, dir);
  }

  return json;
}

}  // namespace Tactile::IO
