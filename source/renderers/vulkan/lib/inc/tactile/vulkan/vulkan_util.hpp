// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <string_view>  // string_view

#include <vulkan/vulkan.h>

#include "tactile/base/prelude.hpp"
#include "tactile/vulkan/api.hpp"

namespace tactile::vk {

[[nodiscard]]
TACTILE_VULKAN_API auto to_string(VkResult result) noexcept -> std::string_view;

}  // namespace tactile::vk
