#ifndef SP_UTIL_SORT_INSERTIONSORT_H
#define SP_UTIL_SORT_INSERTIONSORT_H

#include <cstddef>
#include <util/assert.h>
#include <util/comparator.h>

namespace sp {
//=====================================
template <typename T, typename Comparator = sp::greater>
void
insertionsort(T *, std::size_t) noexcept;

//=====================================
template <typename T, typename Comparator>
void
insertionsort(T *const arr, std::size_t length) noexcept {
  Comparator cmp;
  for (std::size_t i = 1; i < length; ++i) {
    assertx(arr);
    T *current = arr + i;
    std::size_t j = i;
    while (j-- > 0) {
      if (cmp(arr[j], *current)) {
        using std::swap;
        swap(arr[j], *current);
        current = arr + j;
      } else {
        break;
      }
    }
  }
}
}

#endif
