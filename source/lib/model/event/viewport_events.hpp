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
#include "common/type/math.hpp"
#include "core/viewport.hpp"

namespace tactile {

struct ViewportMousePressedEvent final {
  ViewportMouseInfo mouse_info;
};

struct ViewportMouseDraggedEvent final {
  ViewportMouseInfo mouse_info;
};

struct ViewportMouseReleasedEvent final {
  ViewportMouseInfo mouse_info;
};

struct ViewportMouseEnteredEvent final {};

struct ViewportMouseExitedEvent final {};

struct CenterViewportEvent final {};

struct ResetZoomEvent final {};

struct DecreaseZoomEvent final {};

struct IncreaseZoomEvent final {};

struct OffsetDocumentViewportEvent final {
  Float2 delta {};  ///< The offset to apply.
};

struct OffsetTilesetViewportEvent final {
  Entity attached_tileset {kNullEntity};  ///< The target attached tileset entity.
  Float2 delta {};                        ///< The offset to apply.
};

/// Event used to update the limits of tilesets in the tileset dock widget.
struct UpdateTilesetViewportLimitsEvent final {
  Entity attached_tileset {kNullEntity};  ///< The target attached tileset entity.
  Float2 min_offset {};                   ///< The minimum offset.
  Float2 max_offset {};                   ///< The maximum offset.
};

struct PanUpEvent final {};

struct PanDownEvent final {};

struct PanLeftEvent final {};

struct PanRightEvent final {};

}  // namespace tactile