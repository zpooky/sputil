#ifndef SP_UTIL_UTIL_NUMERIC_H
#define SP_UTIL_UTIL_NUMERIC_H

#include <util/swap.h>

void
swap(char &, char &) noexcept;

// void
// swap(signed char &, signed char &) noexcept;
//
// void
// swap(unsigned char &, unsigned char &) noexcept;

void
swap(short &, short &) noexcept;

// void
// swap(signed short &, signed short &) noexcept;
//
// void
// swap(unsigned short &, unsigned short &) noexcept;

void
swap(int &f, int &s) noexcept {
  int tmp = f;
  f = s;
  s = tmp;
}

// void
// swap(signed int &, signed int &) noexcept;
//
// void
// swap(unsigned int &, unsigned int &) noexcept;

// TODO add swaps

#endif
