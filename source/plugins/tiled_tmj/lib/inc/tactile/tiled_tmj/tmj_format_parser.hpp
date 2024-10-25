// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#pragma once

#include <expected>  // expected

#include "tactile/base/debug/error_code.hpp"
#include "tactile/base/io/save/ir.hpp"
#include "tactile/base/io/save/save_format.hpp"
#include "tactile/base/runtime/runtime.hpp"
#include "tactile/tiled_tmj/api.hpp"
#include "tactile/tiled_tmj/tmj_common.hpp"

namespace tactile::tiled_tmj {

[[nodiscard]]
TACTILE_TILED_TMJ_API auto parse_tmj_map(const IRuntime& runtime,
                                         const JSON& map_json,
                                         const SaveFormatReadOptions& options)
    -> std::expected<ir::Map, ErrorCode>;

}  // namespace tactile::tiled_tmj
