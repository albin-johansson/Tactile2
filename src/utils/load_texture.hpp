#pragma once

#include <filesystem>  // path

#include "aliases/ints.hpp"
#include "aliases/maybe.hpp"
#include "core/components/texture.hpp"

namespace Tactile {

void UnloadTextures();

[[nodiscard]] auto LoadTexture(const std::filesystem::path& path) -> Maybe<Texture>;

}  // namespace Tactile
