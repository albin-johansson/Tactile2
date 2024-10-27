// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/godot_tscn/godot_scene_format.hpp"

#include <exception>  // exception

#include "tactile/base/document/map_view.hpp"
#include "tactile/base/io/int_parser.hpp"
#include "tactile/base/numeric/saturate_cast.hpp"
#include "tactile/godot_tscn/gd3_document_converter.hpp"
#include "tactile/godot_tscn/gd3_exporter.hpp"
#include "tactile/godot_tscn/logging.hpp"

namespace tactile::godot_tscn {
namespace {

[[nodiscard]]
auto _deduce_godot_version(const SaveFormatExtraSettings& settings) -> int
{
  int version {3};

  if (const auto iter = settings.find("version"); iter != settings.end()) {
    const auto parsed_version = iter->second.as_int();
    if (parsed_version == 3 || parsed_version == 4) {
      version = parsed_version;
    }
  }

  return version;
}

[[nodiscard]]
auto _deduce_ellipse_polygon_vertices(const SaveFormatExtraSettings& settings) -> std::size_t
{
  std::size_t vertices {32};

  if (const auto iter = settings.find("ellipse_polygon_vertices"); iter != settings.end()) {
    const auto parsed_vertices = saturate_cast<std::size_t>(iter->second.as_int());
    if (parsed_vertices >= 4) {
      vertices = parsed_vertices;
    }
  }

  return vertices;
}

}  // namespace

auto GodotSceneFormat::load_map(const std::filesystem::path&,
                                const SaveFormatReadOptions&) const
    -> std::expected<ir::Map, ErrorCode>
{
  return std::unexpected {ErrorCode::kNotSupported};
}

auto GodotSceneFormat::save_map(const IMapView& map,
                                const SaveFormatWriteOptions& options) const
    -> std::expected<void, ErrorCode>
{
  try {
    const auto version = _deduce_godot_version(options.extra);
    const auto ellipse_polygon_vertices = _deduce_ellipse_polygon_vertices(options.extra);

    if (version == 3) {
      Gd3DocumentConverter converter {options};
      converter.set_ellipse_polygon_vertices(ellipse_polygon_vertices);

      map.accept(converter);

      const auto& gd_map = converter.get_map();
      return save_godot3_scene(gd_map, options);
    }

    return std::unexpected {ErrorCode::kNotSupported};
  }
  catch (const std::exception& error) {
    TACTILE_GODOT_TSCN_ERROR("Unexpected Godot export error: {}", error.what());
  }

  return std::unexpected {ErrorCode::kUnknown};
}

}  // namespace tactile::godot_tscn
