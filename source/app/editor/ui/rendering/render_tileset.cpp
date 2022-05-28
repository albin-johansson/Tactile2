/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2022 Albin Johansson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "render_tileset.hpp"

#include <entt/entity/registry.hpp>
#include <imgui.h>

#include "core/common/ecs.hpp"
#include "core/components/texture.hpp"
#include "core/components/tiles.hpp"
#include "editor/ui/rendering/graphics.hpp"
#include "io/persistence/preferences.hpp"

namespace tactile {

void render_tileset(GraphicsCtx& graphics, const entt::registry& registry)
{
  const auto& tileset = ctx_get<comp::Tileset>(registry);
  const auto& texture = ctx_get<comp::Texture>(registry);
  const auto& uvTileSize = ctx_get<comp::UvTileSize>(registry);

  const auto tw = static_cast<float>(tileset.tile_width);
  const auto th = static_cast<float>(tileset.tile_height);
  const ImVec2 uv{uvTileSize.width, uvTileSize.height};

  for (int32 row = 0; row < tileset.row_count; ++row) {
    for (int32 col = 0; col < tileset.column_count; ++col) {
      const ImVec4 source{static_cast<float>(col * tileset.tile_width),
                          static_cast<float>(row * tileset.tile_height),
                          tw,
                          th};
      const auto position = graphics.from_matrix_to_absolute(row, col);
      graphics.render_translated_image(texture.id, source, position, uv);
    }
  }

  if (io::get_preferences().is_grid_visible()) {
    graphics.set_line_thickness(1.0f);
    graphics.set_draw_color(cen::colors::white.with_alpha(20));
    graphics.render_translated_grid();
  }
}

}  // namespace tactile