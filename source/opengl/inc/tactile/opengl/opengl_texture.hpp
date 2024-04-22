// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/expected.hpp"
#include "tactile/base/int.hpp"
#include "tactile/base/prelude.hpp"
#include "tactile/opengl/api.hpp"
#include "tactile/render/texture.hpp"

namespace tactile {

/**
 * Represents an OpenGL texture.
 */
class TACTILE_OPENGL_API OpenGLTexture final : public ITexture {
 public:
  using id_type = uint;

  /**
   * Loads a texture from an image on disk.
   *
   * \param image_path The path to the image file.
   *
   * \return
   *    A texture if successful; an error code otherwise.
   */
  [[nodiscard]]
  static auto load(const Path& image_path) -> Result<OpenGLTexture>;

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
  auto get_path() const -> const Path& override;

 private:
  id_type mID;
  TextureSize mSize;
  Path mPath;

  OpenGLTexture(id_type id, TextureSize size, Path path);

  void _dispose() noexcept;
};

}  // namespace tactile