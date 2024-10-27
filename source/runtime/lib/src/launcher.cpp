// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/launcher.hpp"

#include <exception>  // exception
#include <utility>    // move
#include <vector>     // vector

#include <imgui.h>

#include "tactile/base/render/renderer.hpp"
#include "tactile/core/debug/exception.hpp"
#include "tactile/core/engine/engine.hpp"
#include "tactile/core/log/logger.hpp"
#include "tactile/core/tactile_app.hpp"
#include "tactile/runtime/command_line_options.hpp"
#include "tactile/runtime/dynamic_library.hpp"
#include "tactile/runtime/plugin_instance.hpp"
#include "tactile/runtime/runtime.hpp"

namespace tactile::runtime {
namespace {

[[nodiscard]]
auto _get_plugin_names(const CommandLineOptions& options) -> std::vector<std::string_view>
{
  std::vector<std::string_view> plugin_names {};

  if (options.load_zlib) {
    plugin_names.emplace_back("tactile-zlib-compression" TACTILE_DLL_EXT);
  }

  if (options.load_zstd) {
    plugin_names.emplace_back("tactile-zstd-compression" TACTILE_DLL_EXT);
  }

  if (options.load_yaml_format) {
    plugin_names.emplace_back("tactile-yaml-format" TACTILE_DLL_EXT);
  }

  if (options.load_tiled_tmj_format) {
    plugin_names.emplace_back("tactile-tiled-tmj" TACTILE_DLL_EXT);
  }

  if (options.load_tiled_tmx_format) {
    plugin_names.emplace_back("tactile-tiled-tmx" TACTILE_DLL_EXT);
  }

  if (options.load_godot_tscn_format) {
    plugin_names.emplace_back("tactile-godot-tscn" TACTILE_DLL_EXT);
  }

  switch (options.renderer_backend) {
    case RendererBackendId::kOpenGL:
      plugin_names.emplace_back("tactile-opengl-renderer" TACTILE_DLL_EXT);
      break;

    case RendererBackendId::kVulkan:
      plugin_names.emplace_back("tactile-vulkan-renderer" TACTILE_DLL_EXT);
      break;
  }

  return plugin_names;
}

[[nodiscard]]
auto _load_plugins(IRuntime& runtime, const CommandLineOptions& options)
    -> std::vector<PluginInstance>
{
  const auto plugin_names = _get_plugin_names(options);

  std::vector<PluginInstance> plugins {};

  for (const auto plugin_name : plugin_names) {
    auto plugin = PluginInstance::load(&runtime, plugin_name);

    if (!plugin) {
      TACTILE_LOG_ERROR("Could not load plugin '{}'", plugin_name);
      continue;
    }

    plugins.push_back(std::move(*plugin));
  }

  return plugins;
}

}  // namespace

auto launch(const int argc, char* argv[]) -> int
{
  try {
    const auto options = parse_command_line_options(argc, argv);
    if (!options.has_value()) {
      return EXIT_FAILURE;
    }

    Runtime runtime {*options};

    const auto plugins [[maybe_unused]] = _load_plugins(runtime, *options);

    const auto* window = runtime.get_window();
    auto* renderer = runtime.get_renderer();

    if (window == nullptr) {
      TACTILE_LOG_ERROR("Window has not been initialized");
      return EXIT_FAILURE;
    }

    if (renderer == nullptr) {
      TACTILE_LOG_ERROR("A renderer has not been installed");
      return EXIT_FAILURE;
    }

    ImGuiMemAllocFunc imgui_alloc_fn {};
    ImGuiMemFreeFunc imgui_free_fn {};
    void* imgui_user_data {};
    runtime.get_imgui_allocator_functions(&imgui_alloc_fn, &imgui_free_fn, &imgui_user_data);

    ImGui::SetAllocatorFunctions(imgui_alloc_fn, imgui_free_fn, imgui_user_data);
    ImGui::SetCurrentContext(renderer->get_imgui_context());

    core::TactileApp app {&runtime};

    core::Engine engine {&app, renderer};
    engine.run();

    return EXIT_SUCCESS;
  }
  catch (const core::Exception& exception) {
    TACTILE_LOG_FATAL("Unhandled exception: {}\n{}", exception.what(), exception.trace());
  }
  catch (const std::exception& exception) {
    TACTILE_LOG_FATAL("Unhandled exception: {}", exception.what());
  }
  catch (...) {
    TACTILE_LOG_FATAL("Unhandled exception");
  }

  return EXIT_FAILURE;
}

}  // namespace tactile::runtime
