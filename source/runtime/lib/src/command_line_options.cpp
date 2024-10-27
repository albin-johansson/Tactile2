// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/command_line_options.hpp"

#include <cstdlib>    // exit, EXIT_SUCCESS
#include <exception>  // exception
#include <iostream>   // cout, cerr
#include <string>     // string

#include <argparse/argparse.hpp>

namespace tactile::runtime {
namespace {

constexpr const char* kUsageHelpMessage =
    R"(Usage: tactile [--help] [--version] [--renderer <opengl|vulkan>] [--lang <en|en_GB|se>]
               [--texture-filter <nearest|linear>] [--mipmaps <on|off>] [--vsync <on|off>]
               [--limit-fps <on|off>] [--zlib <on|off>] [--zstd <on|off>]
               [--yaml-format <on|off>] [--tiled-tmj-format <on|off>]
               [--tiled-tmx-format <on|off>] [--godot-tscn-format <on|off>]
               [--vulkan-validation <on|off>] [--log-level <trc|dbg|inf|wrn|err>]

Options:
  -h, --help           Prints this help message
  -v, --version        Prints the current version
  -r, --renderer       The renderer backend to use (default: "opengl")
  -l, --lang           The display language (default: "en")
  --texture-filter     The texture filtering mode for loaded textures (default: "nearest")
  --mipmaps            Generate mipmaps for loaded textures (default: "on")
  --vsync              Synchronize image presentation with the monitor vertical blanking period (default: "on")
  --limit-fps          Match frame rate with monitor refresh rate (default: "off")
  --zlib               Load Zlib compression format plugin (default: "on")
  --zstd               Load Zstd compression format plugin (default: "on")
  --yaml-format        Load Tiled TMJ save format plugin (default: "on")
  --tiled-tmj-format   Load Tiled TMJ save format plugin (default: "on")
  --tiled-tmx-format   Load Tiled TMX save format plugin (default: "on")
  --godot-tscn-format  Load Godot TSCN save format plugin (default: "on")
  --vulkan-validation  Load Vulkan validation layers (default: "off")
  --log-level          The verbosity of log output (default: "inf"))";

void _add_bool_argument(argparse::ArgumentParser& parser,
                        const std::string_view name,
                        bool& option)
{
  parser.add_argument(name)
      .nargs(1)
      .choices("on", "off")
      .action([&](const std::string& value) { option = value == "on"; });
}

}  // namespace

auto get_default_command_line_options() -> CommandLineOptions
{
  return {
    .log_level = log::LogLevel::kInfo,
    .renderer_backend = RendererBackendId::kOpenGL,
    .renderer_options =
        RendererOptions {
          .texture_filter_mode = TextureFilterMode::kNearest,
          .use_mipmaps = true,
          .use_vsync = true,
          .limit_fps = false,
          .vulkan_validation = false,
        },
    .load_zlib = true,
    .load_zstd = true,
    .load_yaml_format = true,
    .load_tiled_tmj_format = true,
    .load_tiled_tmx_format = true,
    .load_godot_tscn_format = true,
  };
}

auto parse_command_line_options(const int argc, char* argv[])
    -> std::optional<CommandLineOptions>
{
  auto options = get_default_command_line_options();

  argparse::ArgumentParser parser {"tactile",
                                   TACTILE_VERSION_STRING,
                                   argparse::default_arguments::none};
  parser.set_assign_chars("=");

  parser.add_argument("-h", "--help").nargs(0).action([](const std::string&) {
    std::cout << kUsageHelpMessage << '\n';
    std::exit(EXIT_SUCCESS);
  });

  parser.add_argument("-v", "--version").nargs(0).action([](const std::string&) {
    std::cout << TACTILE_VERSION_STRING << '\n';
    std::exit(EXIT_SUCCESS);
  });

  parser.add_argument("-r", "--renderer")
      .nargs(1)
      .choices("opengl", "vulkan")
      .default_value("opengl")
      .action([&](const std::string& value) {
        if (value == "vulkan") {
          options.renderer_backend = RendererBackendId::kVulkan;
        }
        else {
          options.renderer_backend = RendererBackendId::kOpenGL;
        }
      });

  // TODO
  parser.add_argument("-l", "--lang")
      .nargs(1)
      .choices("en", "en_GB", "se")
      .default_value("en");

  parser.add_argument("--log-level")
      .nargs(1)
      .choices("trc", "dbg", "inf", "wrn", "err")
      .default_value("inf")
      .action([&](const std::string& value) {
        if (value == "trc") {
          options.log_level = log::LogLevel::kTrace;
        }
        else if (value == "dbg") {
          options.log_level = log::LogLevel::kDebug;
        }
        else if (value == "inf") {
          options.log_level = log::LogLevel::kInfo;
        }
        else if (value == "wrn") {
          options.log_level = log::LogLevel::kWarn;
        }
        else if (value == "err") {
          options.log_level = log::LogLevel::kError;
        }
      });

  parser.add_argument("--texture-filter")
      .nargs(1)
      .choices("nearest", "linear")
      .default_value("nearest")
      .action([&](const std::string& value) {
        if (value == "linear") {
          options.renderer_options.texture_filter_mode = TextureFilterMode::kLinear;
        }
        else {
          options.renderer_options.texture_filter_mode = TextureFilterMode::kNearest;
        }
      });

  _add_bool_argument(parser, "--mipmaps", options.renderer_options.use_mipmaps);
  _add_bool_argument(parser, "--vsync", options.renderer_options.use_vsync);
  _add_bool_argument(parser, "--limit-fps", options.renderer_options.limit_fps);
  _add_bool_argument(parser, "--zlib", options.load_zlib);
  _add_bool_argument(parser, "--zstd", options.load_zstd);
  _add_bool_argument(parser, "--yaml-format", options.load_yaml_format);
  _add_bool_argument(parser, "--tiled-tmj-format", options.load_tiled_tmj_format);
  _add_bool_argument(parser, "--tiled-tmx-format", options.load_tiled_tmx_format);
  _add_bool_argument(parser, "--godot-tscn-format", options.load_godot_tscn_format);
  _add_bool_argument(parser,
                     "--vulkan-validation",
                     options.renderer_options.vulkan_validation);

  try {
    parser.parse_args(argc, argv);
  }
  catch (const std::exception& error) {
    std::cerr << "ERROR: " << error.what() << '\n';
    return std::nullopt;
  }

  return options;
}

}  // namespace tactile::runtime
