#ifndef SP_UTIL_UTIL_QUADSET_H
#define SP_UTIL_UTIL_QUADSET_H

#include <cstddef>
#include <cstdint>

namespace sp {
//=====================================
struct Quadset {
  using Type = std::uint64_t;

  typename Quadset::Type *buffer;
  std::size_t capacity;

  Quadset(typename Quadset::Type *, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit Quadset(typename Quadset::Type (&)[SIZE]) noexcept;
};

//=====================================
std::uint8_t
test(const Quadset &, std::size_t) noexcept;

//=====================================
std::uint8_t
set(Quadset &, std::size_t, std::uint8_t) noexcept;

//=====================================
/*
 * returns number of uint64_t required to fit $x distinct values
 */
std::size_t
Quadset_number_of_buffer(std::size_t x) noexcept;

//=====================================
void
swap(Quadset &, Quadset &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <std::size_t SIZE>
Quadset::Quadset(typename Quadset::Type (&b)[SIZE]) noexcept
    : Quadset(b, SIZE) {
}

//=====================================
} // namespace sp

#endif
