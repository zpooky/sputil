#include "xorshift.h"
#include <cassert>
#include <util/numeric.h>
// #include <random>
#include <ctime>

// https://en.wikipedia.org/wiki/Xorshift
// http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf
// http://xoroshiro.di.unimi.it/

namespace prng {
Xorshift32::Xorshift32(std::uint32_t seed) noexcept
    : state(seed) {
  assert(seed != 0);
}

Xorshift32::Xorshift32() noexcept
    : state(0) {
  state = std::uint32_t(std::time(nullptr));

  if (state == 0) {
    ++state;
  }
}

void
swap(Xorshift32 &f, Xorshift32 &s) noexcept {
  sp::swap(f.state, s.state);
}

std::uint32_t
random(Xorshift32 &state) noexcept {
  std::uint32_t &x = state.state;
  assert(x != 0);

  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;

  assert(x != 0);

  return x;
}
} // namespace prng

namespace prng {
Xorshift128plus::Xorshift128plus(std::uint64_t f, std::uint64_t s) noexcept
    : state{f, s} {
  assert(f != 0);
  assert(s != 0);
}

Xorshift128plus::Xorshift128plus() noexcept
    : state{0, 0} {
  for (std::size_t i = 0; i < 2; ++i) {
    state[i] = std::uint32_t(std::time(nullptr));
    if (state[i] == 0) {
      ++state[i];
    }
  }
}

void
swap(Xorshift128plus &f, Xorshift128plus &s) noexcept {
  std::swap(f.state[0], s.state[0]);
  std::swap(f.state[1], s.state[1]);
}

std::uint64_t
random(Xorshift128plus &state) noexcept {
  assert(state.state[0] != 0);
  assert(state.state[1] != 0);
  std::uint64_t s1 = state.state[0];
  const std::uint64_t s0 = state.state[1];
  const std::uint64_t result = s0 + s1;

  state.state[0] = s0;
  s1 ^= s1 << 23;                                    // a
  state.state[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
  return result;
}

} // namespace prng
