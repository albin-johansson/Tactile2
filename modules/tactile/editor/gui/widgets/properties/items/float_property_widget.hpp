#pragma once

#include <tactile-base/tactile_std.hpp>

#include "core/property_value.hpp"

namespace Tactile {

[[nodiscard]] auto FloatPropertyWidget(const PropertyValue& property,
                                       float min = 0,
                                       float max = 0) -> Maybe<float>;

}  // namespace Tactile
