#ifndef TACTILE_TACTILE_HPP_
#define TACTILE_TACTILE_HPP_

#include <cstddef>        // size_t
#include <cstdint>        // int{}_t, uint{}_t
#include <functional>     // reference_wrapper, less
#include <map>            // map
#include <optional>       // optional, nullopt_t, nullopt
#include <type_traits>    // is_pointer_v
#include <unordered_map>  // unordered_map

#define TACTILE_DEFAULT_COPY(Class) \
  Class(const Class&) = default;    \
  auto operator=(const Class&)->Class& = default;

#define TACTILE_DELETE_COPY(Class) \
  Class(const Class&) = delete;    \
  auto operator=(const Class&)->Class& = delete;

#define TACTILE_DEFAULT_MOVE(Class)  \
  Class(Class&&) noexcept = default; \
  auto operator=(Class&&) noexcept->Class& = default;

#define TACTILE_DELETE_MOVE(Class)  \
  Class(Class&&) noexcept = delete; \
  auto operator=(Class&&) noexcept->Class& = delete;

namespace tactile {

using CStr = const char*;
using c_str = const char*;

using uchar = unsigned char;
using uint = unsigned int;
using ulonglong = unsigned long long int;
using usize = std::size_t;

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

/// Identifier for attribute contexts (not persistent).
using ContextID = int32;
using context_id = int32;

/// Identifier for component definition types (not persistent).
using ComponentID = int32;
using component_id = int32;

using MapID = int32;
using map_id = int32;

using TilesetID = int32;
using tileset_id = int32;

using LayerID = int32;
using layer_id = int32;

using ObjectID = int32;
using object_id = int32;

using TileID = int32;
using tile_id = int32;

constexpr tile_id empty_tile = 0;

using TileRow = std::vector<tile_id>;
using TileMatrix = std::vector<TileRow>;

template <typename T>
  requires std::is_pointer_v<T>
using NotNull = T;

template <typename K, typename V>
using tree_map = std::map<K, V, std::less<>>;

template <typename K, typename V>
using hash_map = std::unordered_map<K, V>;

template <typename T>
using Ref = std::reference_wrapper<T>;

template <typename T>
using ref = std::reference_wrapper<T>;

template <typename T>
using Maybe = std::optional<T>;

constexpr std::nullopt_t nothing = std::nullopt;

/// \brief Represents the different possible attributes types.
enum class attribute_type {
  string,    ///< A string property.
  integer,   ///< An integer property.
  floating,  ///< A floating-point property.
  boolean,   ///< A boolean property.
  file,      ///< A file path property.
  color,     ///< A color property.
  object     ///< An integer ID property, that refers to a map object.
};

/// \brief Represents the different available map object types.
enum class object_type {
  point,
  rect,
  ellipse
};

/// \brief Represents all different layer types.
enum class layer_type {
  tile_layer,
  object_layer,
  group_layer
};

}  // namespace tactile

#endif  // TACTILE_TACTILE_HPP_
