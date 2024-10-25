// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <filesystem>     // path
#include <unordered_map>  // unordered_map

#include "tactile/base/document/document_visitor.hpp"
#include "tactile/base/id.hpp"
#include "tactile/base/io/byte_stream.hpp"
#include "tactile/base/io/save/save_format.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/tiled_tmj/api.hpp"
#include "tactile/tiled_tmj/tmj_common.hpp"

namespace tactile::tiled_tmj {

/**
 * Provides information about an external TMJ tileset.
 */
struct TmjFormatExternalTilesetData final
{
  /** The file path to the external tileset file. */
  std::filesystem::path path;

  /** The external tileset JSON. */
  JSON json;
};

/**
 * A document visitor that produces Tiled TMJ format JSON objects.
 */
class TACTILE_TILED_TMJ_API TmjFormatSaveVisitor final : public IDocumentVisitor
{
 public:
  /**
   * Creates a visitor.
   *
   * \param runtime The associated runtime, cannot be null.
   * \param options The write options to use.
   */
  TmjFormatSaveVisitor(IRuntime* runtime, SaveFormatWriteOptions options);

  [[nodiscard]]
  auto visit(const IMapView& map) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto visit(const ITilesetView& tileset) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto visit(const ITileView& tile) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto visit(const ILayerView& layer) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto visit(const IObjectView& object) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto visit(const IComponentView& component) -> std::expected<void, ErrorCode> override;

  [[nodiscard]]
  auto get_map_json() const -> const JSON&;

  [[nodiscard]]
  auto get_external_tilesets() const
      -> const std::unordered_map<TileID, TmjFormatExternalTilesetData>&;

 private:
  IRuntime* m_runtime;
  SaveFormatWriteOptions m_options;
  JSON m_map_node {};
  std::unordered_map<TileID, TmjFormatExternalTilesetData> m_external_tileset_nodes {};
  ByteStream m_tile_byte_cache {};

  [[nodiscard]]
  auto _get_tile_json(const ITileView& tile) -> JSON&;

  [[nodiscard]]
  auto _get_tileset_json(const ITilesetView& tileset) -> JSON&;

  [[nodiscard]]
  auto _get_layer_json(const ILayerView& layer) -> JSON&;

  [[nodiscard]]
  static auto _find_layer_json(JSON& root_node, LayerID id) -> JSON*;

  [[nodiscard]]
  auto _find_tileset_json(TileID first_tile_id) -> JSON*;

  [[nodiscard]]
  static auto _find_tile_json(JSON& tileset_json, TileIndex tile_index) -> JSON*;
};

}  // namespace tactile::tiled_tmj
