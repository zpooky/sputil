#ifndef SP_UTIL_UTIL_NUMERIC_H
#define SP_UTIL_UTIL_NUMERIC_H

#include <util/swap.h>

namespace sp {
namespace impl {
namespace swap {
template <typename T>
void
internal_swap(T &f, T &s) noexcept {
  T tmp(std::move(f));
  f = std::move(s);
  s = std::move(tmp);
}
} // namespace swap
} // namespace impl

inline void
swap(char &f, char &s) noexcept {
  impl::swap::internal_swap(f, s);
}

inline void
swap(signed char &f, signed char &s) noexcept {
  impl::swap::internal_swap(f, s);
}

inline void
swap(unsigned char &f, unsigned char &s) noexcept {
  impl::swap::internal_swap(f, s);
}

// inline void
// swap(short &f, short &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }

inline void
swap(signed short &f, signed short &s) noexcept {
  impl::swap::internal_swap(f, s);
}

inline void
swap(unsigned short &f, unsigned short &s) noexcept {
  impl::swap::internal_swap(f, s);
}

// template<>
inline void
swap(int &f, int &s) noexcept {
  impl::swap::internal_swap(f, s);
}

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }

inline void
swap(unsigned int &f, unsigned int &s) noexcept {
  impl::swap::internal_swap(f, s);
}

// template<>
inline void
swap(long &f, long &s) noexcept {
  impl::swap::internal_swap(f, s);
}

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }

inline void
swap(unsigned long &f, unsigned long &s) noexcept {
  impl::swap::internal_swap(f, s);
}



// TODO add swaps
} // namespace sp

#endif
