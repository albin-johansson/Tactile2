// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/vulkan/vulkan_util.hpp"

#include <magic_enum.hpp>

namespace tactile::vk {

auto to_string(const VkResult result) noexcept -> std::string_view
{
  return magic_enum::enum_name(result);
}

}  // namespace tactile::vk
