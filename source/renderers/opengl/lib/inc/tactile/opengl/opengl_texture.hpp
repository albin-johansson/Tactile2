// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/base/render/texture.hpp"
#include "tactile/opengl/api.hpp"

namespace tactile::gl {

/**
 * Represents an OpenGL texture.
 */
class TACTILE_OPENGL_API OpenGLTexture final : public ITexture
{
 public:
  using id_type = unsigned;

  /**
   * Loads a texture from an image on disk.
   *
   * \param image_path The path to the image file.
   * \param options    The renderer options to use.
   *
   * \return
   * A texture if successful; an error code otherwise.
   */
  [[nodiscard]]
  static auto load(const std::filesystem::path& image_path, const RendererOptions& options)
      -> std::expected<OpenGLTexture, ErrorCode>;

  OpenGLTexture() = delete;

  ~OpenGLTexture() noexcept override;

  OpenGLTexture(OpenGLTexture&& other) noexcept;

  auto operator=(OpenGLTexture&& other) noexcept -> OpenGLTexture&;

  TACTILE_DELETE_COPY(OpenGLTexture);

  [[nodiscard]]
  auto get_handle() const -> void* override;

  [[nodiscard]]
  auto get_size() const -> TextureSize override;

  [[nodiscard]]
  auto get_path() const -> const std::filesystem::path& override;

 private:
  id_type m_id;
  TextureSize m_size;
  std::filesystem::path m_path;

  OpenGLTexture(id_type id, TextureSize size, std::filesystem::path path);

  void _dispose() noexcept;
};

}  // namespace tactile::gl
