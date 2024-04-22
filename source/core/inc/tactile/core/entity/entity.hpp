// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <entt/entity/entity.hpp>

#include "tactile/base/prelude.hpp"

namespace tactile {

using EntityID = entt::entity;

inline constexpr EntityID kInvalidEntity = entt::null;

/**
 * Helper type intended to be inherited by tag component types.
 */
struct CTag
{
  char unused [[maybe_unused]];
};

}  // namespace tactile