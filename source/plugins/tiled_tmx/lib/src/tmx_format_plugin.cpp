// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmx/tmx_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/tiled_tmx/logging.hpp"
#include "tactile/tiled_tmx/tmx_save_format.hpp"

namespace tactile::tiled_tmx {

void TmxFormatPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  set_logger(m_runtime->get_logger());

  m_format = std::make_unique<TmxSaveFormat>(m_runtime);
  m_runtime->set_save_format(SaveFormatId::kTiledTmx, m_format.get());
}

void TmxFormatPlugin::unload()
{
  m_runtime->set_save_format(SaveFormatId::kTiledTmx, nullptr);
  m_format.reset();

  set_logger(nullptr);
  m_runtime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) TmxFormatPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::tiled_tmx
