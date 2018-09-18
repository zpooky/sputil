#include "numeric.h"
#include <utility>

namespace sp {
//=====================================
template <typename T>
static void
internal_swap(T &f, T &s) noexcept {
  T tmp(std::move(f));
  f = std::move(s);
  s = std::move(tmp);
}

//=====================================
void
swap(char &f, char &s) noexcept {
  internal_swap(f, s);
}

void
swap(signed char &f, signed char &s) noexcept {
  internal_swap(f, s);
}

void
swap(unsigned char &f, unsigned char &s) noexcept {
  internal_swap(f, s);
}

//=====================================
void
swap(signed short &f, signed short &s) noexcept {
  internal_swap(f, s);
}

void
swap(unsigned short &f, unsigned short &s) noexcept {
  internal_swap(f, s);
}

//=====================================
void
swap(int &f, int &s) noexcept {
  internal_swap(f, s);
}

void
swap(unsigned int &f, unsigned int &s) noexcept {
  internal_swap(f, s);
}

//=====================================
void
swap(long &f, long &s) noexcept {
  internal_swap(f, s);
}

void
swap(unsigned long &f, unsigned long &s) noexcept {
  internal_swap(f, s);
}

//=====================================
} // namespace sp
