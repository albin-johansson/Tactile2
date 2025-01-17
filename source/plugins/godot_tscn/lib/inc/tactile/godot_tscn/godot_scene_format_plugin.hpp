// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <memory>  // unique_ptr

#include "tactile/base/prelude.hpp"
#include "tactile/base/runtime/plugin.hpp"
#include "tactile/godot_tscn/api.hpp"
#include "tactile/godot_tscn/godot_scene_format.hpp"

namespace tactile::godot_tscn {

class TACTILE_GODOT_API GodotSceneFormatPlugin final : public IPlugin
{
 public:
  void load(IRuntime* runtime) override;

  void unload() override;

 private:
  IRuntime* m_runtime {};
  std::unique_ptr<GodotSceneFormat> m_format {};
};

extern "C"
{
  TACTILE_GODOT_API auto tactile_make_plugin() -> IPlugin*;
  TACTILE_GODOT_API void tactile_free_plugin(IPlugin* plugin);
}

}  // namespace tactile::godot_tscn
