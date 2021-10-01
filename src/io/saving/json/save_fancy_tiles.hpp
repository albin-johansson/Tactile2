#pragma once

#include <entt.hpp>    // registry
#include <filesystem>  // path

#include "common/json.hpp"

namespace Tactile::IO {

[[nodiscard]] auto SaveFancyTiles(const entt::registry& registry,
                                  const std::filesystem::path& dir) -> JSON;

}  // namespace Tactile::IO