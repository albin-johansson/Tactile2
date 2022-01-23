#pragma once

namespace tactile {

/**
 * \brief Invokes the supplied callable for the specified amount of times.
 *
 * \ingroup core
 *
 * \tparam Integer the integral type that represents the number of invocations.
 * \tparam T the type of the callable.
 *
 * \param n the amount of times the callable should be invoked.
 * \param callable the callable that will be invoked.
 */
template <typename Integer, typename T>
constexpr void invoke_n(const Integer n, T&& callable) noexcept(noexcept(callable()))
{
  for (Integer i = 0; i < n; ++i) {
    callable();
  }
}

}  // namespace tactile
