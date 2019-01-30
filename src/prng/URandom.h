#ifndef SP_UTIL_PRNG_U_RANDOM_H
#define SP_UTIL_PRNG_U_RANDOM_H

#include <cstdint>

namespace prng {
/*
 * TODO URandomBuffered internal buffer to reduce the amount of read syscalls
 */
struct URandom {
  using Word = std::uint32_t;
  int mfd;
  /**/
  URandom() noexcept;

  URandom(const URandom &) = delete;
  URandom(const URandom &&) = delete;

  ~URandom() noexcept;

  URandom &
  operator=(const URandom &) = delete;
  URandom &
  operator=(const URandom &&) = delete;
};

void
swap(URandom &, URandom &) noexcept;

std::uint32_t
random(URandom &) noexcept;
} // namespace prng

#endif
