// Copyright (C) 2024 Albin Johansson (GNU General Public License v3.0)

#include "tactile/core/numeric/random.hpp"

#include <concepts>  // same_as, floating_point
#include <random>  // mt19937, random_device, uniform_real_distribution, uniform_int_distribution
#include <thread>  // this_thread

#include "tactile/core/debug/assert.hpp"
#include "tactile/core/logging.hpp"

namespace tactile::core {
namespace {

using RandomEngine = std::mt19937;

/** Creates a seeded pseudo-random number generation engine. */
[[nodiscard]]
auto _make_random_engine() -> RandomEngine
{
  std::random_device entropy_source {};
  const auto seed = entropy_source();

  TACTILE_CORE_DEBUG("Thread {} uses RNG seed {}", std::this_thread::get_id(), seed);

  return RandomEngine {seed};
}

/** Returns the RNG engine associated with the current thread. */
[[nodiscard]]
auto _get_random_engine() -> RandomEngine&
{
  thread_local auto engine = _make_random_engine();
  return engine;
}

/** Returns a random value in the range [min, max]. */
template <typename T>
[[nodiscard]] auto _get_random(const T min, const T max) -> T
{
  static_assert(!std::same_as<T, bool>);
  static_assert(!std::same_as<T, char>);
  static_assert(!std::same_as<T, unsigned char>);

  TACTILE_ASSERT(min <= max);
  auto& engine = _get_random_engine();

  if constexpr (std::floating_point<T>) {
    return std::uniform_real_distribution<T> {min, max}(engine);
  }
  else {
    return std::uniform_int_distribution<T> {min, max}(engine);
  }
}

}  // namespace

void init_random_number_generator()
{
  (void) _get_random_engine();
}

auto get_random_int(const int min, const int max) -> int
{
  return _get_random(min, max);
}

auto get_random_uint(const unsigned min, const unsigned max) -> unsigned
{
  return _get_random(min, max);
}

auto get_random_float(const float min, const float max) -> float
{
  return _get_random(min, max);
}

auto get_random_float_normalized() -> float
{
  return _get_random(0.0f, 1.0f);
}

auto get_random_bool() -> bool
{
  return _get_random(0, 1) == 1;
}

}  // namespace tactile::core
