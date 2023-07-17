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

#include "model/objects/object_system.hpp"

#include <doctest/doctest.h>

#include "model/contexts/context_components.hpp"
#include "model/model_factory.hpp"
#include "model/objects/object_components.hpp"
#include "model/objects/object_factory.hpp"

using namespace tactile;

TEST_SUITE("ObjectSystem")
{
  TEST_CASE("duplicate_object")
  {
    auto model = sys::create_model(BackendAPI::Null);

    const auto old_object_entity = sys::create_object(model, ObjectType::Rect);
    auto& old_object = model.get<Object>(old_object_entity);
    auto& old_object_context = model.get<Context>(old_object_entity);

    old_object.position = Float2 {12, 34};
    old_object.size = Float2 {120, 100};
    old_object.visible = false;
    old_object.tag = "tag";
    old_object.meta_id = 42;

    old_object_context.name = "Cool object";
    old_object_context.props["answer"] = 42;
    old_object_context.props["tint"] = Color {0xFF, 0, 0};

    const auto new_object_entity = sys::duplicate_object(model, old_object_entity);
    const auto& new_object = model.get<Object>(new_object_entity);
    const auto& new_object_context = model.get<Context>(new_object_entity);

    CHECK(new_object.type == old_object.type);
    CHECK(new_object.position == old_object.position);
    CHECK(new_object.size == old_object.size);
    CHECK(new_object.visible == old_object.visible);
    CHECK(new_object.tag == old_object.tag);
    CHECK(!new_object.meta_id.has_value());
    CHECK(old_object.meta_id == 42);

    CHECK(new_object_context.name == old_object_context.name);
    CHECK(new_object_context.props == old_object_context.props);
    CHECK(new_object_context.comps.size() == old_object_context.comps.size());
  }
}