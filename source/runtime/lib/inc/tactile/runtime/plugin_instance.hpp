// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <memory>       // unique_ptr
#include <optional>     // optional
#include <string_view>  // string_view

#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/plugin.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/runtime/api.hpp"

namespace tactile::runtime {

class IDynamicLibrary;

/**
 * Represents a loaded plugin instance.
 */
class TACTILE_RUNTIME_API PluginInstance final
{
 public:
  TACTILE_DELETE_COPY(PluginInstance);

  /**
   * Unloads the associated plugin.
   */
  ~PluginInstance() noexcept;

  PluginInstance(PluginInstance&& other) noexcept;

  auto operator=(PluginInstance&&) -> PluginInstance& = delete;

  /**
   * Attempts to load a plugin.
   *
   * \details
   * This function will first try to load a shared library with the specified
   * name. Then, the "constructor" and "destructor" functions will be looked up
   * in the library (see \c PluginConstructor and \c PluginDestructor), which
   * must be called \c tactile_make_plugin and \c tactile_free_plugin,
   * respectively. The constructor function is then used to create the actual
   * plugin instance, which is then finally initialized via \c IPlugin::load.
   *
   * \details
   * The plugin is unregistered via \c IPlugin::unload and subsequently
   * destroyed when the \c PluginInstance goes out of scope.
   *
   * \param runtime     The associated runtime.
   * \param plugin_name The file name of the plugin module.
   *
   * \return
   * A plugin instance if successful; an empty optional otherwise.
   */
  [[nodiscard]]
  static auto load(IRuntime* runtime, std::string_view plugin_name)
      -> std::optional<PluginInstance>;

 private:
  IRuntime* m_runtime;
  std::unique_ptr<IDynamicLibrary> m_dll;
  PluginDestructor* m_plugin_destructor;
  IPlugin* m_plugin;
  bool m_primed;

  PluginInstance(IRuntime* runtime,
                 std::unique_ptr<IDynamicLibrary> dll,
                 PluginDestructor* plugin_destructor,
                 IPlugin* plugin);
};

}  // namespace tactile::runtime
