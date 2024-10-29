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
  : m_runtime {runtime},
    m_dll {std::move(dll)},
    m_plugin_destructor {plugin_destructor},
    m_plugin {plugin},
    m_primed {true}
{}

PluginInstance::~PluginInstance() noexcept
{
  if (m_primed) {
    m_plugin->unload();
    m_plugin_destructor(m_plugin);
    m_primed = false;
  }
}

PluginInstance::PluginInstance(PluginInstance&& other) noexcept
  : m_runtime {std::exchange(other.m_runtime, nullptr)},
    m_dll {std::exchange(other.m_dll, nullptr)},
    m_plugin_destructor {std::exchange(other.m_plugin_destructor, nullptr)},
    m_plugin {std::exchange(other.m_plugin, nullptr)},
    m_primed {std::exchange(other.m_primed, false)}
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
