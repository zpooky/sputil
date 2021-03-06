#ifndef SP_UTIL_SORT_INSERTIONSORT_H
#define SP_UTIL_SORT_INSERTIONSORT_H

#include <cstddef>
#include <util/assert.h>
#include <util/comparator.h>
#include <utility>

namespace sp {
//=====================================
template <typename T, typename Comparator = sp::less>
void
insertionsort(T *, std::size_t) noexcept;

//=====================================
template <typename T, typename Comparator>
void
insertionsort(T *const arr, std::size_t length) noexcept {
  Comparator cmp;

  for (std::size_t i = 1; i < length; ++i) {
    assertxs(arr, length);
    T *current = arr + i;
    std::size_t j = i;

    while (j-- > 0) {
      if (cmp(*current, arr[j])) {
        using std::swap;
        swap(arr[j], *current);
        current = arr + j;
      } else {
        break;
      }
    } // while
  }   // for
}

//=====================================
} // namespace sp

#endif
