// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/opengl/opengl_texture.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <bit>      // bit_cast
#include <cstdint>  // uintptr_t
#include <utility>  // move, exchange

#include <glad/glad.h>
#include <stb_image.h>

#include "tactile/base/render/renderer_options.hpp"
#include "tactile/opengl/opengl_error.hpp"

namespace tactile {

auto OpenGLTexture::load(const std::filesystem::path& image_path,
                         const RendererOptions& options)
    -> std::expected<OpenGLTexture, std::error_code>
{
  unsigned texture_id {};

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  const auto filter_mode =
      options.texture_filter_mode == TextureFilterMode::kNearest ? GL_NEAREST : GL_LINEAR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  if (const auto err = glGetError(); err != GL_NONE) {
    return std::unexpected {make_error(map_opengl_error_code(err))};
  }

  TextureSize texture_size {};
  auto* pixel_data = stbi_load(image_path.string().c_str(),
                               &texture_size.width,
                               &texture_size.height,
                               nullptr,
                               STBI_rgb_alpha);
  if (!pixel_data) {
    return std::unexpected {make_error(OpenGLError::kBadImage)};
  }

  glTexImage2D(GL_TEXTURE_2D,
               0,                    // LOD index
               GL_RGBA,              // Internal image format
               texture_size.width,   // Width
               texture_size.height,  // Height
               0,                    // Border (must be 0)
               GL_RGBA,              // Pixel data format
               GL_UNSIGNED_BYTE,     // Data type of pixels
               pixel_data);
  stbi_image_free(pixel_data);

  if (options.use_mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  if (const auto err = glGetError(); err != GL_NONE) {
    return std::unexpected {make_error(map_opengl_error_code(err))};
  }

  return OpenGLTexture {texture_id, texture_size, image_path};
}

OpenGLTexture::OpenGLTexture(const id_type id,
                             const TextureSize size,
                             std::filesystem::path path)
  : mID {id},
    mSize {size},
    mPath {std::move(path)}
{}

OpenGLTexture::OpenGLTexture(OpenGLTexture&& other) noexcept
  : mID {std::exchange(other.mID, 0)},
    mSize {std::exchange(other.mSize, TextureSize {})},
    mPath {std::exchange(other.mPath, std::filesystem::path {})}
{}

auto OpenGLTexture::operator=(OpenGLTexture&& other) noexcept -> OpenGLTexture&
{
  if (this != &other) {
    _dispose();

    mID = std::exchange(other.mID, 0);
    mSize = std::exchange(other.mSize, TextureSize {});
    mPath = std::exchange(other.mPath, std::filesystem::path {});
  }

  return *this;
}

OpenGLTexture::~OpenGLTexture() noexcept
{
  _dispose();
}

void OpenGLTexture::_dispose() noexcept
{
  if (mID != 0) {
    glDeleteTextures(1, &mID);
    mID = 0;
  }
}

auto OpenGLTexture::get_handle() const -> void*
{
  return std::bit_cast<void*>(static_cast<std::uintptr_t>(mID));
}

auto OpenGLTexture::get_size() const -> TextureSize
{
  return mSize;
}

auto OpenGLTexture::get_path() const -> const std::filesystem::path&
{
  return mPath;
}

}  // namespace tactile
