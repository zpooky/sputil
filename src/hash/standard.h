#ifndef SP_UTIL_HASH_STANDARD_H
#define SP_UTIL_HASH_STANDARD_H

#include <cstddef>

namespace sp {
template <typename>
struct Hasher;

struct Equality {
  template <typename F, typename S>
  bool
  operator()(const F &f, const S &s) const noexcept {
    return f == s;
  }
};

//=====================================
template <>
struct Hasher<bool> {
  std::size_t
  operator()(bool) const noexcept;
};

//=====================================
template <>
struct Hasher<signed char> {
  std::size_t
  operator()(signed char) const noexcept;
};

//=====================================
template <>
struct Hasher<unsigned char> {
  std::size_t
  operator()(unsigned char) const noexcept;
};

//=====================================
template <>
struct Hasher<signed short> {
  std::size_t
  operator()(signed short) const noexcept;
};

//=====================================
template <>
struct Hasher<unsigned short> {
  std::size_t
  operator()(unsigned short) const noexcept;
};

//=====================================
template <>
struct Hasher<signed int> {
  std::size_t
  operator()(signed int) const noexcept;
};

//=====================================
template <>
struct Hasher<unsigned int> {
  std::size_t
  operator()(unsigned int) const noexcept;
};

//=====================================
template <>
struct Hasher<signed long> {
  std::size_t
  operator()(signed long) const noexcept;
};

//=====================================
template <>
struct Hasher<unsigned long> {
  std::size_t
  operator()(unsigned long) const noexcept;
};
}

#endif
