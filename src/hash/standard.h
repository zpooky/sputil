#ifndef SP_UTIL_HASH_STANDARD_H
#define SP_UTIL_HASH_STANDARD_H

#include <cstddef>
#include <type_traits>

namespace sp {
//=====================================
template <typename>
struct Hasher;

//=====================================
template <typename T>
struct Equality {

  template <typename S>
  bool
  operator()(const T &f, const S &s) const noexcept {
    static_assert(!std::is_pointer<T>::value, "Can not be pointer");
    return f == s;
  }
};

//=====================================
template <typename T>
struct PointerEquality {

  template <typename S>
  bool
  operator()(const T *f, const S &s) const noexcept {
    // static_assert(std::is_pointer<T>::value, "Must be a pointer");
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
struct Hasher<char> {
  std::size_t
  operator()(char) const noexcept;
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

#if 0
//=====================================
template <typename T>
template <>
struct Hasher<T *> {
  std::size_t
  operator()(const T *) const noexcept;
};
#endif
} // namespace sp

#endif
