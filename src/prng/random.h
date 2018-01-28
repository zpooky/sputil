#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <cstdint>
#include <utility>

namespace sp {

// template <typename Random>
// std::uint32_t
// uniform_dist(Random &, std::uint32_t, std::uint32_t) noexcept;

/*
 * =======================================================
 */

template <typename Random,
          typename T = decltype(random(std::declval<Random>()))>
T
uniform_dist(Random &r, T inc, T ex) noexcept {
  assert(inc >= T(0));
  assert(inc < ex);
  T res{random(r)};
  return (res % (ex - inc)) + inc;
}
} // namespace sp
#endif
