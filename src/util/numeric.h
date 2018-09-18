#ifndef SP_UTIL_UTIL_NUMERIC_H
#define SP_UTIL_UTIL_NUMERIC_H

#include <limits>
#include <type_traits>
#include <util/swap.h>

namespace sp {
//=====================================
template <typename T, typename T2>
bool overflow_sum(T, T2) noexcept;

//=====================================
void
swap(char &f, char &s) noexcept;

void
swap(signed char &f, signed char &s) noexcept;

void
swap(unsigned char &f, unsigned char &s) noexcept;

// inline void
// swap(short &f, short &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }
//=====================================
void
swap(signed short &f, signed short &s) noexcept;

void
swap(unsigned short &f, unsigned short &s) noexcept;

//=====================================
void
swap(int &f, int &s) noexcept;

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }
void
swap(unsigned int &f, unsigned int &s) noexcept;

//=====================================
void
swap(long &f, long &s) noexcept;

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }
void
swap(unsigned long &f, unsigned long &s) noexcept;
// TODO add swaps

//=====================================
//====Implementation===================
//=====================================
template <typename T, typename T2>
bool
overflow_sum(T first, T2 second) noexcept {
  static_assert(std::is_same<T, T2>::value, "");
  static_assert(std::is_integral<T>::value, "");
  constexpr T MAX = std::numeric_limits<T>::max();

  return first > (MAX - second);
}

//=====================================
} // namespace sp

#endif
