#include "map_system.hpp"

#include <cmath>     // abs
#include <concepts>  // invocable, unsigned_integral

#include "core/algorithms/invoke_n.hpp"
#include "core/components/layer.hpp"
#include "core/components/tile_layer.hpp"
#include "core/map.hpp"
#include "core/systems/layers/tile_layer_system.hpp"

namespace Tactile::Sys {
namespace {

template <std::unsigned_integral T>
[[nodiscard]] constexpr auto GetDiff(const T a, const T b) noexcept -> T
{
  if (a < b) {
    return b - a;
  }
  else {
    return a - b;
  }
}

template <std::invocable<TileLayer&> T>
void TileLayerQuery(entt::registry& registry, T&& invocable)
{
  for (auto&& [entity, layer] : registry.view<Layer>().each()) {
    if (auto* tileLayer = registry.try_get<TileLayer>(entity)) {
      invocable(*tileLayer);
    }
  }
}

}  // namespace

void AddRow(entt::registry& registry)
{
  auto& map = registry.ctx<Map>();
  ++map.row_count;

  TileLayerQuery(registry, [nCols = map.column_count](TileLayer& layer) {
    layer.matrix.push_back(MakeTileRow(nCols, empty_tile));
  });
}

void AddColumn(entt::registry& registry)
{
  auto& map = registry.ctx<Map>();
  ++map.column_count;

  TileLayerQuery(registry, [](TileLayer& layer) {
    for (auto& row : layer.matrix) {
      row.push_back(empty_tile);
    }
  });
}

void RemoveRow(entt::registry& registry)
{
  auto& map = registry.ctx<Map>();
  if (map.row_count > 1) {
    --map.row_count;
    TileLayerQuery(registry, [](TileLayer& layer) { layer.matrix.pop_back(); });
  }
}

void RemoveColumn(entt::registry& registry)
{
  auto& map = registry.ctx<Map>();
  if (map.column_count > 1) {
    --map.column_count;
    TileLayerQuery(registry, [](TileLayer& layer) {
      for (auto& row : layer.matrix) {
        assert(row.size() > 1);
        row.pop_back();
      }
    });
  }
}

void ResizeMap(entt::registry& registry, const usize nRows, const usize nCols)
{
  auto& map = registry.ctx<Map>();

  if (const auto diff = GetDiff(map.row_count, nRows); map.row_count < nRows) {
    InvokeN(diff, [&] { AddRow(registry); });
  }
  else {
    InvokeN(diff, [&] { RemoveRow(registry); });
  }

  if (const auto diff = GetDiff(map.column_count, nCols); map.column_count < nCols) {
    InvokeN(diff, [&] { AddColumn(registry); });
  }
  else {
    InvokeN(diff, [&] { RemoveColumn(registry); });
  }
}

auto IsPositionInMap(const entt::registry& registry, const MapPosition& position) -> bool
{
  const auto& map = registry.ctx<Map>();

  const auto row = position.GetRow();
  const auto column = position.GetColumn();

  return (row >= 0) && (column >= 0) && (static_cast<usize>(row) < map.row_count) &&
         (static_cast<usize>(column) < map.column_count);
}

}  // namespace Tactile::Sys
