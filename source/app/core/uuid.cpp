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

#include "uuid.hpp"

#include <functional>  // hash

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_hash.hpp>

namespace tactile {

auto make_uuid() -> boost::uuids::uuid
{
  static boost::uuids::random_generator generator;
  return generator();
}

auto hash(const UUID& uuid) -> usize
{
  const std::hash<UUID> hasher {};
  return hasher(uuid);
}

}  // namespace tactile
