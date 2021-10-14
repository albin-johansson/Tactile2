#pragma once

#include <vector>  // vector

#include "common/expected.hpp"
#include "common/json.hpp"
#include "io/parsing/parse_error.hpp"
#include "io/parsing/parse_ir.hpp"

namespace Tactile::IO {

[[nodiscard]] auto ParseFancyTiles(const JSON& json)
    -> Expected<std::vector<TileData>, ParseError>;

}  // namespace Tactile::IO
