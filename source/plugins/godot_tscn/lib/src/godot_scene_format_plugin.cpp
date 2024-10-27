// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/godot_tscn/godot_scene_format_plugin.hpp"

#include <new>  // nothrow

#include "tactile/base/runtime/runtime.hpp"
#include "tactile/godot_tscn/logging.hpp"

namespace tactile::godot_tscn {

void GodotSceneFormatPlugin::load(IRuntime* runtime)
{
  m_runtime = runtime;
  set_logger(m_runtime->get_logger());

  m_format = std::make_unique<GodotSceneFormat>();
  m_runtime->set_save_format(SaveFormatId::kGodotTscn, m_format.get());
}

void GodotSceneFormatPlugin::unload()
{
  m_runtime->set_save_format(SaveFormatId::kGodotTscn, nullptr);
  m_format.reset();

  set_logger(nullptr);
  m_runtime = nullptr;
}

auto tactile_make_plugin() -> IPlugin*
{
  return new (std::nothrow) GodotSceneFormatPlugin {};
}

void tactile_free_plugin(IPlugin* plugin)
{
  delete plugin;
}

}  // namespace tactile::godot_tscn
