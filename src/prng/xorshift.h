#ifndef SP_UTIL_PRNG_XORSHIFT_H
#define SP_UTIL_PRNG_XORSHIFT_H

#include <cstdint>

namespace prng {
struct Xorshift32 {
  using Word = std::uint32_t;
  using InitType = std::uint32_t;

  std::uint32_t state;

  explicit Xorshift32(InitType) noexcept;
  Xorshift32() noexcept;
};

void
swap(Xorshift32 &, Xorshift32 &) noexcept;

std::uint32_t
random(Xorshift32 &) noexcept;

} // namespace prng

namespace prng {
struct Xorshift128plus {
  using Word = std::uint64_t;
  using InitType = std::uint64_t[2];

  std::uint64_t state[2];

  Xorshift128plus(std::uint64_t, std::uint64_t) noexcept;
  explicit Xorshift128plus(const InitType) noexcept;
  Xorshift128plus() noexcept;
};

void
swap(Xorshift128plus &, Xorshift128plus &) noexcept;

std::uint64_t
random(Xorshift128plus &) noexcept;
} // namespace prng

#endif
