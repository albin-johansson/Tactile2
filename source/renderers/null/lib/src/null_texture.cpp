// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/null_renderer/null_texture.hpp"

#include <utility>  // move

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace tactile {

NullTexture::NullTexture(const TextureSize size, std::filesystem::path path)
  : m_size {size},
    m_path {std::move(path)}
{}

auto NullTexture::load(std::filesystem::path path) -> std::expected<NullTexture, ErrorCode>
{
  TextureSize size {};

  const auto path_string = path.string();
  auto* pixels =
      stbi_load(path_string.c_str(), &size.width, &size.height, nullptr, STBI_default);

  if (!pixels) {
    return std::unexpected {ErrorCode::kBadImage};
  }

  stbi_image_free(pixels);

  return NullTexture {size, std::move(path)};
}

auto NullTexture::get_handle() const -> void*
{
  return nullptr;
}

auto NullTexture::get_size() const -> TextureSize
{
  return m_size;
}

auto NullTexture::get_path() const -> const std::filesystem::path&
{
  return m_path;
}

}  // namespace tactile
