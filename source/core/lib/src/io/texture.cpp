// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/io/texture.hpp"

#include <system_error>  // make_error_code, errc

#include "tactile/base/render/renderer.hpp"
#include "tactile/base/render/texture.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {

auto load_texture(IRenderer& renderer, const std::filesystem::path& path)
    -> std::expected<CTexture, ErrorCode>
{
  const auto path_string = path.string();

  const auto texture_id = renderer.load_texture(path_string.c_str());
  if (!texture_id.has_value()) {
    TACTILE_CORE_ERROR("Could not load texture '{}': {}",
                      path_string,
                      to_string(texture_id.error()));
    return std::unexpected {texture_id.error()};
  }

  const auto* texture = renderer.find_texture(*texture_id);
  if (!texture) {
    TACTILE_CORE_ERROR("Could not find loaded texture");
    return std::unexpected {ErrorCode::kBadState};
  }

  const auto texture_size = texture->get_size();

  return CTexture {
    .raw_handle = texture->get_handle(),
    .id = *texture_id,
    .size = Int2 {texture_size.width, texture_size.height},
    .path = path,
  };
}

}  // namespace tactile::core
