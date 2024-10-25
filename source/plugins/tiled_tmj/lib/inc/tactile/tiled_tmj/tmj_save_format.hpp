// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/io/save/save_format.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/tiled_tmj/api.hpp"

namespace tactile::tiled_tmj {

/**
 * Implements the Tiled TMJ save format.
 *
 * \see https://doc.mapeditor.org/en/stable/reference/json-map-format/
 */
class TACTILE_TILED_TMJ_API TmjSaveFormat final : public ISaveFormat
{
 public:
  explicit TmjSaveFormat(IRuntime* runtime);

  [[nodiscard]]
  auto load_map(const std::filesystem::path& map_path,
                const SaveFormatReadOptions& options) const
      -> std::expected<ir::Map, ErrorCode> override;

  [[nodiscard]]
  auto save_map(const IMapView& map, const SaveFormatWriteOptions& options) const
      -> std::expected<void, ErrorCode> override;

 private:
  IRuntime* m_runtime;
};

}  // namespace tactile::tiled_tmj
