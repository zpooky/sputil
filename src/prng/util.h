#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <prng/URandom.h>
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

template <typename Random>
void
fill(Random &r, void *buffer, const std::size_t size) noexcept {
  std::size_t idx = 0;
  unsigned char *it = (unsigned char *)buffer;
  while (idx < size) {
    auto stuff = random(r);
    if (stuff != 0) {
      const std::size_t len = std::min(size - idx, sizeof(stuff));
      std::memcpy(it + idx, &stuff, len);
      idx += len;
    }
  }
}

template <typename Random, std::size_t SIZE>
void
fill(Random &r, unsigned char (&buffer)[SIZE]) noexcept {
  fill(r, buffer, SIZE);
}

template <typename Random>
Random
seed() noexcept {
  typename Random::InitType init{};
  URandom r;
  fill(r, &init, sizeof(init));

  return Random{init};
}

} // namespace prng
#endif
