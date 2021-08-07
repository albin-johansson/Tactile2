#pragma once

#include "aliases/czstring.hpp"
#include "aliases/not_null.hpp"

namespace Tactile {

/**
 * \brief Shows a combo box.
 *
 * \ingroup gui
 *
 * \param label the label associated with the combo box.
 * \param values the combo box options separated by `\0` and terminated by `\0\0`.
 * \param[out] index where the index of the currently selected item is written.
 * \param tooltip optional tooltip text.
 *
 * \return `true` if the selected item changed; `false` otherwise.
 */
auto Combo(NotNull<czstring> label,
           NotNull<czstring> values,
           NotNull<int*> index,
           czstring tooltip = nullptr) -> bool;

}  // namespace Tactile