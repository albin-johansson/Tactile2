// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmx/tmx_save_format.hpp"

#include <exception>  // exception

#include "tactile/base/document/map_view.hpp"
#include "tactile/base/document/meta_view.hpp"
#include "tactile/tiled_tmx/logging.hpp"
#include "tactile/tiled_tmx/tmx_common.hpp"
#include "tactile/tiled_tmx/tmx_format_parser.hpp"
#include "tactile/tiled_tmx/tmx_format_save_visitor.hpp"

namespace tactile::tiled_tmx {

TmxSaveFormat::TmxSaveFormat(IRuntime* runtime)
  : m_runtime {runtime}
{}

auto TmxSaveFormat::load_map(const std::filesystem::path& map_path,
                             const SaveFormatReadOptions& options) const
    -> std::expected<ir::Map, ErrorCode>
{
  try {
    return parse_map(*m_runtime, map_path, options);
  }
  catch (const std::exception& error) {
    TACTILE_TILED_TMX_ERROR("An unexpected error occurred during TMX map parsing: {}",
                            error.what());
  }
  catch (...) {
    TACTILE_TILED_TMX_ERROR("An unknown error occurred during TMX map parsing");
  }

  return std::unexpected {ErrorCode::kUnknown};
}

auto TmxSaveFormat::save_map(const IMapView& map, const SaveFormatWriteOptions& options) const
    -> std::expected<void, ErrorCode>
{
  try {
    const auto* map_path = map.get_path();

    if (!map_path) {
      TACTILE_TILED_TMX_ERROR("Map has no associated file path");
      return std::unexpected {ErrorCode::kBadState};
    }

    TmxFormatSaveVisitor saver {m_runtime, options};

    return map.accept(saver).and_then([&]() -> std::expected<void, ErrorCode> {
      const auto& map_document = saver.get_map_xml_document();
      const auto& external_tileset_documents = saver.get_tileset_xml_documents();

      auto write_result = save_xml_document(map_document, *map_path);
      if (!write_result.has_value()) {
        return write_result;
      }

      for (const auto& [relative_path, tileset_document] : external_tileset_documents) {
        const auto tileset_path = options.base_dir / relative_path;
        write_result = save_xml_document(tileset_document, tileset_path);

        if (!write_result.has_value()) {
          return write_result;
        }
      }

      return {};
    });
  }
  catch (const std::exception& error) {
    TACTILE_TILED_TMX_ERROR("An unexpected error occurred during TMX map emission: {}",
                            error.what());
  }
  catch (...) {
    TACTILE_TILED_TMX_ERROR("An unknown error occurred during TMX map emission");
  }

  return std::unexpected {ErrorCode::kUnknown};
}

}  // namespace tactile::tiled_tmx
