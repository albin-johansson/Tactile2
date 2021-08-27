#pragma once

#include <nenya.hpp>  // strong_type

#include "ints.hpp"

namespace Tactile {

namespace Tags {
struct ObjectIdTag;
}  // namespace Tags

using object_id = nenya::strong_type<int, Tags::ObjectIdTag>;

[[nodiscard]] constexpr auto operator""_obj(const ulonglong value) noexcept
    -> object_id
{
  return object_id{static_cast<object_id::value_type>(value)};
}

}  // namespace Tactile
