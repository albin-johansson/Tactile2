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

#pragma once

#include "common/type/ecs.hpp"
#include "common/type/string.hpp"
#include "model/components/component_components.hpp"
#include "model/model.hpp"

namespace tactile::sys {

/**
 * Creates a new empty component definition.
 *
 * \param model         the associated model.
 * \param component_set the associated component set.
 * \param name          the name of the component.
 *
 * \return a component definition entity.
 */
auto create_component(Model& model, ComponentSet& component_set, String name) -> Entity;

}  // namespace tactile::sys