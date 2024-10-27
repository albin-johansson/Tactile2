// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/tiled_tmj/tmj_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/tiled_tmj/logging.hpp"
#include "tactile/tiled_tmj/tmj_save_format.hpp"

namespace tactile::tiled_tmj {

void TmjFormatPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  set_logger(m_runtime->get_logger());

  m_format = std::make_unique<TmjSaveFormat>(m_runtime);
  m_runtime->set_save_format(SaveFormatId::kTiledTmj, m_format.get());
}

void TmjFormatPlugin::unload()
{
  m_runtime->set_save_format(SaveFormatId::kTiledTmj, nullptr);
  m_format.reset();

  set_logger(nullptr);
  m_runtime = nullptr;
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
