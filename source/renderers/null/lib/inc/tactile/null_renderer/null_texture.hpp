// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/render/texture.hpp"
#include "tactile/null_renderer/api.hpp"

namespace tactile {

/**
 * A null texture implementation.
 */
class TACTILE_NULL_RENDERER_API NullTexture final : public ITexture
{
 public:
  /**
   * Attempts to load a texture.
   *
   * \note
   * The texture will actually be loaded to memory to deduce the texture size.
   *
   * \param path The texture path.
   *
   * \return
   * A texture if successful; an error code otherwise.
   */
  [[nodiscard]]
  static auto load(std::filesystem::path path) -> std::expected<NullTexture, ErrorCode>;

  [[nodiscard]]
  auto get_handle() const -> void* override;

  [[nodiscard]]
  auto get_size() const -> TextureSize override;

  [[nodiscard]]
  auto get_path() const -> const std::filesystem::path& override;

 private:
  TextureSize m_size;
  std::filesystem::path m_path;

  NullTexture(TextureSize size, std::filesystem::path path);
};

}  // namespace tactile
