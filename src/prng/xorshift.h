#ifndef SP_UTIL_PRNG_XORSHIFT_H
#define SP_UTIL_PRNG_XORSHIFT_H

#include <cstdint>

namespace prng {
struct xorshift32 {
  using Word = std::uint32_t;
  using InitType = std::uint32_t;

  std::uint32_t state;

  explicit xorshift32(InitType) noexcept;
  xorshift32() noexcept;
};

void
swap(xorshift32 &, xorshift32 &) noexcept;

std::uint32_t
random(xorshift32 &) noexcept;

} // namespace prng

namespace prng {
struct xorshift128plus {
  using Word = std::uint64_t;
  using InitType = std::uint64_t[2];

  std::uint64_t state[2];

  xorshift128plus(std::uint64_t, std::uint64_t) noexcept;
  explicit xorshift128plus(const InitType) noexcept;
  xorshift128plus() noexcept;
};

void
swap(xorshift128plus &, xorshift128plus &) noexcept;

std::uint64_t
random(xorshift128plus &) noexcept;

} // namespace prng

#endif
