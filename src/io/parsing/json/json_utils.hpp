#pragma once

#include "common/cstr.hpp"
#include "common/json.hpp"
#include "common/maybe.hpp"

namespace Tactile::IO {

template <typename T>
[[nodiscard]] auto GetValue(const JSON& json, const CStr key) -> Maybe<T>
{
  if (const auto it = json.find(key); it != json.end()) {
    return it->get<T>();
  }
  else {
    return nothing;
  }
}

}  // namespace Tactile::IO
