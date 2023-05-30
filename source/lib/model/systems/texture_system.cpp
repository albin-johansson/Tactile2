/*
 * This source file is a part of the Tactile map editor.
 *
 * Copyright (C) 2023 Albin Johansson
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

#include "texture_system.hpp"

#include "components/texture.hpp"
#include "io/texture_loader.hpp"
#include "model/systems/validation_system.hpp"

namespace tactile::sys {

void destroy_loaded_texture_resources(Model& model)
{
  const auto& texture_callbacks = model.get<TextureCallbacks>();
  const auto& texture_cache = model.get<TextureCache>();

  for (const auto& [texture_path, texture_entity]: texture_cache.textures) {
    texture_callbacks.destroy(model, texture_entity);
  }
}

auto create_texture(Model& model, const Path& path) -> Entity
{
  auto& texture_cache = model.get<TextureCache>();

  // Check if the image has already been loaded, if so just return the associated entity.
  if (const auto iter = texture_cache.textures.find(path);
      iter != texture_cache.textures.end()) {
    return iter->second;
  }

  const auto& texture_callbacks = model.get<TextureCallbacks>();
  const auto texture_data = load_texture_data(path).value();

  const auto texture_entity = model.create_entity();
  texture_cache.textures[path] = texture_entity;

  auto& texture = model.add<Texture>(texture_entity);
  texture.path = path;
  texture.size = texture_data.size;

  texture_callbacks.init(model, texture_entity, texture_data);

  return texture_entity;
}

}  // namespace tactile::sys
