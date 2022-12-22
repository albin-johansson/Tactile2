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

#include "core/type/path.hpp"
#include "core/type/uuid.hpp"

namespace tactile {

/// Event emitted when a loaded document should be opened in its own tab.
struct OpenDocumentEvent final {
  UUID document_id {};
};

struct CloseDocumentEvent final {
  UUID id {};
};

struct SelectDocumentEvent final {
  UUID id {};
};

struct SaveEvent final {};

struct SaveAsEvent final {
  Path path;
};

struct OpenSaveAsDialogEvent final {};

}  // namespace tactile