// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_save_format.hpp"

#include "tactile/base/document/map_view.hpp"
#include "tactile/tiled_tmj/logging.hpp"
#include "tactile/tiled_tmj/tmj_format_parser.hpp"
#include "tactile/tiled_tmj/tmj_format_save_visitor.hpp"

namespace tactile::tiled_tmj {

TmjSaveFormat::TmjSaveFormat(IRuntime* runtime)
  : m_runtime {runtime}
{}

auto TmjSaveFormat::load_map(const std::filesystem::path& map_path,
                             const SaveFormatReadOptions& options) const
    -> std::expected<ir::Map, ErrorCode>
{
  try {
    return read_json_document(map_path).and_then([this, &options](const JSON& map_json) {
      return parse_tmj_map(*m_runtime, map_json, options);
    });
  }
  catch (const std::exception& error) {
    TACTILE_TILED_TMJ_ERROR("An unexpected error occurred during TMJ map parsing: {}",
                            error.what());
    return std::unexpected {ErrorCode::kParseError};
  }
  catch (...) {
    TACTILE_TILED_TMJ_ERROR("An unknown error occurred during TMJ map parsing");
    return std::unexpected {ErrorCode::kUnknown};
  }
}

auto TmjSaveFormat::save_map(const IMapView& map, const SaveFormatWriteOptions& options) const
    -> std::expected<void, ErrorCode>
{
  try {
    const auto* map_path = map.get_path();

    if (!map_path) {
      TACTILE_TILED_TMJ_ERROR("Map has no associated file path");
      return std::unexpected {ErrorCode::kBadState};
    }

    TACTILE_TILED_TMJ_DEBUG("Saving TMJ map to {}", map_path->string());

    TmjFormatSaveVisitor visitor {m_runtime, options};

    return map.accept(visitor)
        .and_then([&] {
          const auto& map_json = visitor.get_map_json();
          return save_json_document(*map_path, map_json, options.use_indentation ? 2 : 0);
        })
        .and_then([&]() -> std::expected<void, ErrorCode> {
          const auto& external_tilesets = visitor.get_external_tilesets();

          for (const auto& [first_tiled_id, external_tileset] : external_tilesets) {
            const auto save_tileset_result =
                save_json_document(external_tileset.path,
                                   external_tileset.json,
                                   options.use_indentation ? 2 : 0);
            if (!save_tileset_result.has_value()) {
              TACTILE_TILED_TMJ_ERROR("Could not save external tileset");
              return std::unexpected {save_tileset_result.error()};
            }
          }

          return {};
        });
  }
  catch (const std::exception& error) {
    TACTILE_TILED_TMJ_ERROR("An error occurred during TMJ map emission: {}", error.what());
    return std::unexpected {ErrorCode::kWriteError};
  }
  catch (...) {
    TACTILE_TILED_TMJ_ERROR("An unknown error occurred during TMJ map emission");
    return std::unexpected {ErrorCode::kUnknown};
  }
}

}  // namespace tactile::tiled_tmj
