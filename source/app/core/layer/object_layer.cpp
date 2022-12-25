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

#include "object_layer.hpp"

#include <utility>  // move

#include "core/debug/panic.hpp"
#include "core/util/assoc.hpp"

namespace tactile {
namespace {

[[nodiscard]] auto get_bounds(const Object& object, const Float2& tile_size) -> Float4
{
  if (object.is_point()) {
    return {object.get_pos() - (tile_size * 0.25f), tile_size * 0.5f};
  }
  else {
    return {object.get_pos(), object.get_size()};
  }
}

}  // namespace

void ObjectLayer::accept(ContextVisitor& visitor) const
{
  visitor.visit(*this);
}

void ObjectLayer::accept(LayerVisitor& visitor)
{
  visitor.visit(*this);
}

void ObjectLayer::accept(ConstLayerVisitor& visitor) const
{
  visitor.visit(*this);
}

void ObjectLayer::add_object(Shared<Object> object)
{
  const auto id = object->get_uuid();
  mObjects[id] = std::move(object);
}

void ObjectLayer::remove_object(const UUID& id)
{
  mObjects.erase(id);

  if (mActiveObject == id) {
    mActiveObject.reset();
  }
}

void ObjectLayer::reserve_objects(const usize n)
{
  mObjects.reserve(n);
}

void ObjectLayer::select_object(const Maybe<UUID>& id)
{
  mActiveObject = id;
  if (mActiveObject && !has_object(*mActiveObject)) {
    throw TactileError {"Invalid object identifier!"};
  }
}

auto ObjectLayer::has_object(const UUID& id) const -> bool
{
  return mObjects.find(id) != mObjects.end();
}

auto ObjectLayer::object_count() const -> usize
{
  return mObjects.size();
}

auto ObjectLayer::active_object_id() const -> Maybe<UUID>
{
  return mActiveObject;
}

auto ObjectLayer::get_object_ptr(const UUID& id) -> const Shared<Object>&
{
  return lookup_in(mObjects, id);
}

auto ObjectLayer::get_object(const UUID& id) -> Object&
{
  return *lookup_in(mObjects, id);
}

auto ObjectLayer::get_object(const UUID& id) const -> const Object&
{
  return *lookup_in(mObjects, id);
}

auto ObjectLayer::object_at(const Float2& pos, const Float2& tile_size) const
    -> Maybe<UUID>
{
  for (const auto& [id, object]: mObjects) {
    const auto bounds = get_bounds(*object, tile_size);

    const auto max_x = bounds.x + bounds.z;
    const auto max_y = bounds.y + bounds.w;

    if (pos.x >= bounds.x && pos.y >= bounds.y && pos.x <= max_x && pos.y <= max_y) {
      return id;
    }
  }

  return nothing;
}

auto ObjectLayer::clone() const -> Shared<Layer>
{
  auto copy = std::make_shared<ObjectLayer>();
  copy->mDelegate = mDelegate.clone();

  // FIXME clone objects

  return copy;
}

}  // namespace tactile
