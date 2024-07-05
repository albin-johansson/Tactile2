// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include "tactile/base/container/string.hpp"
#include "tactile/base/prelude.hpp"

namespace tactile {

/**
 * Event for opening the component editor dialog.
 */
struct ShowComponentEditorDialogEvent final
{};

/**
 * Event for creating a new component.
 */
struct CreateComponentEvent final
{
  String name;
};

}  // namespace tactile