#ifndef SP_UTIL_UTIL_NUMERIC_H
#define SP_UTIL_UTIL_NUMERIC_H

#include <util/swap.h>
#include <utility>

namespace sp {

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
/*
 * ===============================================
 */

void
swap(signed short &f, signed short &s) noexcept;

void
swap(unsigned short &f, unsigned short &s) noexcept;
/*
 * ===============================================
 */

void
swap(int &f, int &s) noexcept;

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }
void
swap(unsigned int &f, unsigned int &s) noexcept;
/*
 * ===============================================
 */
void
swap(long &f, long &s) noexcept;

// inline void
// swap(signed int &f, signed int &s) noexcept {
//   impl::swap::internal_swap(f, s);
// }
void
swap(unsigned long &f, unsigned long &s) noexcept;
// TODO add swaps
} // namespace sp

#endif
