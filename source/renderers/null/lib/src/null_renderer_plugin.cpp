// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/null_renderer/null_renderer_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/runtime/logging.hpp"

namespace tactile::null_renderer {

void NullRendererPlugin::load(IRuntime* runtime)
{
  runtime::log(LogLevel::kTrace, "Loading null renderer plugin");
  m_runtime = runtime;

  m_runtime->init_window(0);
  auto* window = m_runtime->get_window();

  if (!window) {
    runtime::log(LogLevel::kError, "Could not initialize window");
    return;
  }

  m_renderer = std::make_unique<NullRenderer>(window);
  m_runtime->set_renderer(m_renderer.get());
}

void NullRendererPlugin::unload()
{
  runtime::log(LogLevel::kTrace, "Unloading null renderer plugin");

  m_runtime->set_renderer(nullptr);
  m_runtime = nullptr;

  m_renderer.reset();
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) NullRendererPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::null_renderer
