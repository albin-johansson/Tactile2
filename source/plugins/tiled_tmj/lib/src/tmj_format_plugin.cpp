// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/runtime/logging.hpp"
#include "tactile/tiled_tmj/tmj_save_format.hpp"

namespace tactile::tiled_tmj {

void TmjFormatPlugin::load(IRuntime* runtime)
{
  runtime::log(LogLevel::kTrace, "Loading Tiled TMJ format plugin");
  m_runtime = runtime;

  m_format = std::make_unique<TmjSaveFormat>(m_runtime);
  m_runtime->set_save_format(SaveFormatId::kTiledTmj, m_format.get());
}

void TmjFormatPlugin::unload()
{
  runtime::log(LogLevel::kTrace, "Unloading Tiled TMJ format plugin");

  m_runtime->set_save_format(SaveFormatId::kTiledTmj, nullptr);
  m_runtime = nullptr;

  m_format.reset();
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) TmjFormatPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::tiled_tmj
