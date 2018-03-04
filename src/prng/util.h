#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <cstdint>
#include <cstring>
#include <utility>

namespace prng {

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
fill(Random &r, void *buffer, std::size_t size) noexcept;

template <typename Random, std::size_t SIZE>
void
fill(Random &r, unsigned char (&buffer)[SIZE]) noexcept {
  fill(r, buffer, SIZE);
}

template <typename Random>
Random
seed() noexcept;

} // namespace prng
#endif
