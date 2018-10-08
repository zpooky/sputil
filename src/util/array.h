#ifndef SP_UTIL_UTIL_ARRAY_H
#define SP_UTIL_UTIL_ARRAY_H

#include <cstddef>
#include <cstdint>
#include <util/assert.h>

namespace sp {
//=====================================
template <typename T>
std::size_t
index_of(const T *arr, std::size_t length, std::size_t capacity,
         const T *needle) noexcept;

namespace n {
//=====================================
template <typename T, typename K>
const T *
search(const T *arr, std::size_t length, const K *needle) noexcept;

//=====================================
template <typename T, typename K>
T *
search(T *arr, std::size_t length, const K *needle) noexcept;
}

template <typename T>
std::size_t
distance(const T *, const T *) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
std::size_t
index_of(const T *const arr, std::size_t l, std::size_t cap,
         const T *ptr) noexcept {
  assertxs(arr, l, cap);
  assertxs(ptr, l, cap);

  if (ptr) {
    auto s = reinterpret_cast<std::uintptr_t>(arr);
    auto e = reinterpret_cast<std::uintptr_t>(ptr);

    if (e >= s) {
      auto index = e - s;
      // assertx(index % sizeof(*ptr) == 0);??
      index /= sizeof(*ptr);

      if (index < l) {
        return index;
      }
    }
  }

  return cap;
}

namespace n {
//=====================================
template <typename T, typename K>
const T *
search(const T *arr, std::size_t length, const K *needle) noexcept {
  const T *it = arr;
  const T *const end = it + length;
  while (it != end) {
    if (*it == *needle) {
      return it;
    }
    ++it;
  }

  return nullptr;
}

//=====================================
template <typename T, typename K>
T *
search(T *arr, std::size_t length, const K *needle) noexcept {
  const T *const c_arr = arr;
  return (T *)search(c_arr, length, needle);
}
}

//=====================================
template <typename T>
std::size_t
distance(const T *f, const T *s) noexcept {
  assertx(f);
  assertx(s);
  auto fl = reinterpret_cast<std::uintptr_t>(f);
  auto sl = reinterpret_cast<std::uintptr_t>(s);
  assertxs(fl <= sl, fl, sl);

  if (fl <= sl) {
    return sl - fl;
  }

  return 0;
}
}

#endif
