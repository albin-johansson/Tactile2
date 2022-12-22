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

#include "parse_map.hpp"

#include <exception>  // exception

#include <spdlog/spdlog.h>

#include "core/util/fmt.hpp"
#include "io/map/parse/json/json_parser.hpp"
#include "io/map/parse/xml/xml_parser.hpp"
#include "io/map/parse/yaml/yaml_parser.hpp"
#include "misc/panic.hpp"
#include "misc/profile.hpp"

namespace tactile::io {

auto parse_map(const Path& path) -> ParseResult
{
  spdlog::info("Parsing map {}", path);
  ParseResult result;

  try {
    TACTILE_PROFILE_START

    if (!fs::exists(path)) {
      result.set_error(ParseError::MapDoesNotExist);
      return result;
    }

    const auto ext = path.extension();
    if (ext == ".yaml" || ext == ".yml") {
      result = parse_yaml_map(path);
    }
    else if (ext == ".xml" || ext == ".tmx") {
      result = parse_xml_map(path);
    }
    else if (ext == ".json" || ext == ".tmj") {
      result = parse_json_map(path);
    }
    else {
      spdlog::error("Unsupported save file extension: {}", ext);
      result.set_error(ParseError::UnsupportedMapExtension);
      return result;
    }

    TACTILE_PROFILE_END("Parsed map")
  }
  catch (const TactileError& e) {
    result.set_error(ParseError::Unknown);
    spdlog::error("Parser threw unhandled exception with message: '{}'\n{}",
                  e.what(),
                  e.get_trace());
  }
  catch (const std::exception& e) {
    result.set_error(ParseError::Unknown);
    spdlog::error("Parser threw unhandled exception with message: '{}'\n", e.what());
  }
  catch (...) {
    spdlog::error("Parser threw non-exception value!");
    result.set_error(ParseError::Unknown);
  }

  return result;
}

}  // namespace tactile::io