// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan_renderer/vulkan_util.hpp"

#include <magic_enum.hpp>

namespace tactile {

auto to_string(const VkResult result) noexcept -> StringView
{
  return magic_enum::enum_name(result);
}

}  // namespace tactile