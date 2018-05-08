#ifndef SP_UTIL_SORT_UTIL_H
#define SP_UTIL_SORT_UTIL_H

#include <cstddef>
#include <util/comparator.h>

namespace sp {
//=====================================
template <typename T, typename Comparator = sp::greater>
bool
is_sorted(T *, std::size_t) noexcept;
//=====================================

template <typename T, typename Comparator>
bool
is_sorted(T *arr, std::size_t length) noexcept {
  Comparator cmp;

  T *priv = arr;
  for (std::size_t i = 1; i < length; ++i) {
    if (!cmp(arr[i], *priv)) {
      return false;
    }
    priv = arr + i;
  }

  return true;
}
//=====================================
}

#endif
