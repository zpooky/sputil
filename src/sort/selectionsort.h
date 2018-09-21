#ifndef SP_UTIL_SORT_SELECTIONSORT_H
#define SP_UTIL_SORT_SELECTIONSORT_H

#include <cstddef>
#include <util/assert.h>
#include <util/comparator.h>
#include <utility>

namespace sp {

//=====================================
template <typename T, typename C = sp::less>
void
selectionsort(T *, std::size_t) noexcept;

//=====================================
//====Implementation===================
//=====================================

template <typename T, typename C>
void
selectionsort(T *const in, std::size_t length) noexcept {
  if (length == 0) {
    return;
  }
  assertxs(in, length);

  const T *const end = in + length;
  C cmp;

  for (T *start = in; start != end; ++start) {
    T *min = start;

    for (T *it = start + 1; it != end; ++it) {
      if (cmp(*it, /*<*/ *min)) {
        min = it;
      }
    } // for

    if (min != start) {
      using std::swap;
      swap(*min, *start);
    }
  } // for
}
} // namespace sp

#endif
