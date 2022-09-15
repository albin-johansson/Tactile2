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

#pragma once

#include <string>  // string

#include "core/common/ints.hpp"
#include "core/common/macros.hpp"
#include "core/common/math.hpp"
#include "core/common/maybe.hpp"
#include "core/common/memory.hpp"
#include "core/ctx/context.hpp"
#include "core/ctx/context_delegate.hpp"
#include "core/layer/object_type.hpp"

namespace tactile {

/// Represents various map objects found in object layers.
/// The current supported object types are rectangles, ellipses, and points.
class Object final : public IContext {
 public:
  TACTILE_DEFAULT_COPY(Object);
  TACTILE_DEFAULT_MOVE(Object);

  explicit Object(ObjectType type = ObjectType::Rect);

  void set_pos(const float2& pos);
  void set_size(const float2& size);

  /// Changes the type of the object.
  /// When converting to point, both width and height are set to zero.
  void set_type(ObjectType type);

  void set_tag(std::string tag);

  void set_meta_id(int32 id);

  void set_visible(bool visible);

  void set_name(std::string name) override;

  void accept(IContextVisitor& visitor) const override;

  [[nodiscard]] auto get_props() -> PropertyBundle& override;
  [[nodiscard]] auto get_props() const -> const PropertyBundle& override;

  [[nodiscard]] auto get_comps() -> ComponentBundle& override;
  [[nodiscard]] auto get_comps() const -> const ComponentBundle& override;

  [[nodiscard]] auto get_uuid() const -> const UUID& override;

  [[nodiscard]] auto get_name() const -> const std::string& override;

  [[nodiscard]] auto get_type() const -> ObjectType;

  [[nodiscard]] auto is_rect() const -> bool;
  [[nodiscard]] auto is_ellipse() const -> bool;
  [[nodiscard]] auto is_point() const -> bool;

  [[nodiscard]] auto get_pos() const -> const float2&;
  [[nodiscard]] auto get_size() const -> const float2&;

  [[nodiscard]] auto get_tag() const -> const std::string&;

  [[nodiscard]] auto get_meta_id() const -> Maybe<int32>;

  [[nodiscard]] auto is_visible() const -> bool;

  /// Creates a copy of the object, with a new UUID.
  [[nodiscard]] auto clone() const -> Shared<Object>;

 private:
  ContextDelegate mDelegate;
  float2 mPos {};            ///< Object position.
  float2 mSize {};           ///< Object size (might be zero).
  ObjectType mType;          ///< Specific object type.
  std::string mTag;          ///< Optional user-provided tag.
  Maybe<int32> mMetaId;      ///< Identifier used in save files.
  bool mVisible : 1 {true};  ///< Is the object rendered?
};

}  // namespace tactile
