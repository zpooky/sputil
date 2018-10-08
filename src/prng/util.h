#ifndef SP_UTIL_PRNG_RANDOM_H
#define SP_UTIL_PRNG_RANDOM_H

#include <cstdint>
#include <cstring>
#include <util/assert.h>
#include <utility>

namespace prng {
//=====================================
template <typename Random>
typename Random::Word
uniform_dist(Random &r, typename Random::Word inc,
             typename Random::Word ex) noexcept;

//=====================================
template <typename Random>
bool
uniform_bool(Random &r) noexcept;

//=====================================
template <typename Random>
using dist_f = char (*)(Random &);

template <typename Random>
void
fill(Random &r, void *buffer, std::size_t size, dist_f<Random>) noexcept;

//=====================================
template <typename Random>
void
fill(Random &r, void *buffer, std::size_t size) noexcept;

template <typename Random, std::size_t SIZE>
void
fill(Random &r, unsigned char (&buffer)[SIZE]) noexcept {
  fill(r, buffer, SIZE);
}

//=====================================
template <typename Random>
Random
seed() noexcept;

//=====================================
} // namespace prng

#endif
