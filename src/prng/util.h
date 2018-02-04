#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <utility>

namespace prng {
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

template <typename Random, std::size_t SIZE>
void
fill(Random &r, unsigned char (&buffer)[SIZE]) noexcept {
  std::size_t idx = 0;
  while (idx < SIZE) {
    auto stuff = random(r);
    const std::size_t len = std::min(SIZE - idx, sizeof(stuff));
    std::memcpy(buffer + idx, &stuff, len);
    idx += len;
  }
}

} // namespace random
#endif
