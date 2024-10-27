// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <cstdint>   // uint8_t
#include <format>    // formatter, format_to
#include <optional>  // optional

#include "tactile/base/prelude.hpp"
#include "tactile/base/render/renderer_options.hpp"
#include "tactile/log/log_level.hpp"
#include "tactile/runtime/api.hpp"

namespace tactile::runtime {

enum class RendererBackendId : std::uint8_t
{
  kOpenGL,
  kVulkan,
};

struct CommandLineOptions final
{
  log::LogLevel log_level;
  RendererBackendId renderer_backend;
  RendererOptions renderer_options;
  bool load_zlib;
  bool load_zstd;
  bool load_yaml_format;
  bool load_tiled_tmj_format;
  bool load_tiled_tmx_format;
  bool load_godot_tscn_format;
};

[[nodiscard]]
TACTILE_RUNTIME_API auto get_default_command_line_options() -> CommandLineOptions;

[[nodiscard]]
TACTILE_RUNTIME_API auto parse_command_line_options(int argc, char* argv[])
    -> std::optional<CommandLineOptions>;

}  // namespace tactile::runtime

template <>
struct std::formatter<tactile::runtime::RendererBackendId> final
{
  template <typename FormatParseContext>
  constexpr static auto parse(FormatParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const tactile::runtime::RendererBackendId& id, FormatContext& ctx) const
  {
    switch (id) {
      case tactile::runtime::RendererBackendId::kOpenGL:
        return std::format_to(ctx.out(), "opengl");

      case tactile::runtime::RendererBackendId::kVulkan:
        return std::format_to(ctx.out(), "vulkan");

      default: return std::format_to(ctx.out(), "?");
    }
  }
};
