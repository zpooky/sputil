#include "xorshift.h"
#include <util/assert.h>
#include <util/numeric.h>
// #include <random>
#include <cstdio>
#include <ctime>

// https://en.wikipedia.org/wiki/Xorshift
// http://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf
// http://xoroshiro.di.unimi.it/

namespace prng {
xorshift32::xorshift32(InitType seed) noexcept
    : state(seed) {
  assertx(seed != 0);
}

xorshift32::xorshift32() noexcept
    : state(0) {
  state = std::uint32_t(std::time(nullptr));

  if (state == 0) {
    ++state;
  }
}

void
swap(xorshift32 &f, xorshift32 &s) noexcept {
  sp::swap(f.state, s.state);
}

std::uint32_t
random(xorshift32 &state) noexcept {
  std::uint32_t &x = state.state;
  assertx(x != 0);

  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;

  assertx(x != 0);

  return x;
}
} // namespace prng

namespace prng {
xorshift128plus::xorshift128plus(std::uint64_t f, std::uint64_t s) noexcept
    : state{f, s} {
  assertx(f != 0);
  assertx(s != 0);
}

xorshift128plus::xorshift128plus(const InitType init) noexcept
    : xorshift128plus{init[0], init[1]} {
}

xorshift128plus::xorshift128plus() noexcept
    : state{0, 0} {
  for (std::size_t i = 0; i < 2; ++i) {
    state[i] = std::uint32_t(std::time(nullptr));
    if (state[i] == 0) {
      ++state[i];
    }
  }
}

void
swap(xorshift128plus &f, xorshift128plus &s) noexcept {
  std::swap(f.state[0], s.state[0]);
  std::swap(f.state[1], s.state[1]);
}

std::uint64_t
random(xorshift128plus &state) noexcept {
  assertx(state.state[0] != 0);
  assertx(state.state[1] != 0);
  std::uint64_t s1 = state.state[0];
  const std::uint64_t s0 = state.state[1];
  const std::uint64_t result = s0 + s1;

  state.state[0] = s0;
  s1 ^= s1 << 23;                                    // a
  state.state[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); // b, c
  return result;
}

} // namespace prng
