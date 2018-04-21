#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <cstdint>
#include <cstring>
#include <util/assert.h>
#include <utility>

namespace prng {

template <typename Random>
typename Random::Word
uniform_dist(Random &r, typename Random::Word inc,
             typename Random::Word ex) noexcept {

  assertx(inc >= 0);
  assertx(inc < ex);
  typename Random::Word res{random(r)};
  return (res % (ex - inc)) + inc;
}

template <typename Random>
bool
uniform_bool(Random &r) noexcept {
  auto res = uniform_dist(r, 0, 2);
  return res == 1 ? true : false;
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
