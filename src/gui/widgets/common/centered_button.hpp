#pragma once

#include "common/cstr.hpp"
#include "common/not_null.hpp"

namespace Tactile {

/**
 * \brief Shows a button that is centered in the parent region.
 *
 * \ingroup gui
 *
 * \param label the button label.
 *
 * \return `true` if the button was pressed; `false` otherwise.
 */
auto CenteredButton(NotNull<CStr> label) -> bool;

}  // namespace Tactile
