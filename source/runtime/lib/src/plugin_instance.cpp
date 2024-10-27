// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/runtime/plugin_instance.hpp"

#include <utility>  // move, exchange

#include "tactile/runtime/dynamic_library.hpp"
#include "tactile/runtime/logging.hpp"

namespace tactile::runtime {

PluginInstance::PluginInstance(IRuntime* runtime,
                               std::unique_ptr<IDynamicLibrary> dll,
                               PluginDestructor* plugin_destructor,
                               IPlugin* plugin)
  : mRuntime {runtime},
    mDLL {std::move(dll)},
    mPluginDestructor {plugin_destructor},
    mPlugin {plugin},
    mPrimed {true}
{}

PluginInstance::~PluginInstance() noexcept
{
  if (mPrimed) {
    mPlugin->unload();
    mPluginDestructor(mPlugin);
    mPrimed = false;
  }
}

PluginInstance::PluginInstance(PluginInstance&& other) noexcept
  : mRuntime {std::exchange(other.mRuntime, nullptr)},
    mDLL {std::exchange(other.mDLL, nullptr)},
    mPluginDestructor {std::exchange(other.mPluginDestructor, nullptr)},
    mPlugin {std::exchange(other.mPlugin, nullptr)},
    mPrimed {std::exchange(other.mPrimed, false)}
{}

auto PluginInstance::load(IRuntime* runtime, const std::string_view plugin_name)
    -> std::optional<PluginInstance>
{
  auto dll = load_library(plugin_name);

  if (!dll) {
    TACTILE_RUNTIME_ERROR("Could not load plugin '{}'", plugin_name);
    return std::nullopt;
  }

  auto* plugin_ctor = find_symbol<PluginConstructor>(*dll, "tactile_make_plugin");
  auto* plugin_dtor = find_symbol<PluginDestructor>(*dll, "tactile_free_plugin");

  if (!plugin_ctor || !plugin_dtor) {
    TACTILE_RUNTIME_ERROR("Plugin '{}' has incompatible API", plugin_name);
    return std::nullopt;
  }

  auto* plugin = plugin_ctor();

  if (!plugin) {
    TACTILE_RUNTIME_ERROR("Could not initialize plugin '{}'", plugin_name);
    return std::nullopt;
  }

  TACTILE_RUNTIME_DEBUG("Loading plugin '{}'", plugin_name);
  plugin->load(runtime);

  return PluginInstance {runtime, std::move(dll), plugin_dtor, plugin};
}

}  // namespace tactile::runtime
