// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/document/map_document.hpp"

#include <optional>  // optional
#include <utility>   // move

#include "tactile/base/io/save/ir.hpp"
#include "tactile/core/document/document_info.hpp"
#include "tactile/core/document/map_view_impl.hpp"
#include "tactile/core/entity/registry.hpp"
#include "tactile/core/logging.hpp"
#include "tactile/core/map/map.hpp"
#include "tactile/core/map/map_spec.hpp"
#include "tactile/core/tile/tileset_types.hpp"
#include "tactile/core/ui/viewport.hpp"
#include "tactile/core/util/uuid.hpp"

namespace tactile::core {

struct MapDocument::Data final
{
  UUID uuid;
  Registry registry;
  EntityID map_entity;
  std::optional<std::filesystem::path> path;
  SaveFormatId format;

  Data()
    : uuid {UUID::generate()},
      registry {},
      map_entity {kInvalidEntity},
      path {std::nullopt},
      format {SaveFormatId::kTactileYaml}
  {
    registry.add<CTileCache>();
    registry.add<CDocumentInfo>();
  }
};

MapDocument::MapDocument()
  : mData {std::make_unique<Data>()}
{}

auto MapDocument::make(const MapSpec& spec) -> std::expected<MapDocument, ErrorCode>
{
  MapDocument document {};
  auto& registry = document.mData->registry;

  const auto map_id = make_map(registry, spec);
  if (map_id == kInvalidEntity) {
    TACTILE_CORE_ERROR("Could not create map document");
    return std::unexpected {ErrorCode::kBadInit};
  }

  document.mData->map_entity = map_id;

  auto& document_info = registry.get<CDocumentInfo>();
  document_info.root = map_id;
  document_info.active_context = map_id;
  document_info.tile_size = spec.tile_size;

  return document;
}

auto MapDocument::make(IRenderer& renderer, const ir::Map& ir_map)
    -> std::expected<MapDocument, ErrorCode>
{
  MapDocument document {};
  auto& registry = document.mData->registry;

  const auto map_id = make_map(registry, renderer, ir_map);
  if (!map_id.has_value()) {
    TACTILE_CORE_ERROR("Could not create map document: {}", to_string(map_id.error()));
    return std::unexpected {map_id.error()};
  }

  document.mData->map_entity = *map_id;
  const auto& map = registry.get<CMap>(*map_id);

  auto& document_info = registry.get<CDocumentInfo>();
  document_info.root = *map_id;
  document_info.active_context = *map_id;
  document_info.tile_size = map.tile_size;

  return document;
}

TACTILE_DEFINE_MOVE(MapDocument);

MapDocument::~MapDocument() noexcept = default;

auto MapDocument::accept(IDocumentVisitor& visitor) const -> std::expected<void, ErrorCode>
{
  const MapViewImpl map_view {this};
  return map_view.accept(visitor);
}

void MapDocument::update()
{}

void MapDocument::set_path(std::filesystem::path path)
{
  mData->path = std::move(path);
}

void MapDocument::set_format(const SaveFormatId format_id)
{
  mData->format = format_id;
}

auto MapDocument::get_path() const -> const std::filesystem::path*
{
  return mData->path.has_value() ? &mData->path.value() : nullptr;
}

auto MapDocument::get_format() const -> SaveFormatId
{
  return mData->format;
}

auto MapDocument::get_registry() -> Registry&
{
  return mData->registry;
}

auto MapDocument::get_registry() const -> const Registry&
{
  return mData->registry;
}

auto MapDocument::get_content_size() const -> Float2
{
  const auto map_entity = mData->map_entity;

  const auto& map = mData->registry.get<CMap>(map_entity);
  const auto& viewport = mData->registry.get<CViewport>(map_entity);

  return Float2 {static_cast<float>(map.extent.cols), static_cast<float>(map.extent.rows)} *
         vec_cast<Float2>(map.tile_size) * viewport.scale;
}

auto MapDocument::get_tile_size() const -> Int2
{
  const auto& map = mData->registry.get<CMap>(mData->map_entity);
  return map.tile_size;
}

auto MapDocument::get_extent() const -> Extent2D
{
  const auto& map = mData->registry.get<CMap>(mData->map_entity);
  return map.extent;
}

auto MapDocument::get_uuid() const -> const UUID&
{
  return mData->uuid;
}

}  // namespace tactile::core
