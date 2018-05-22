#ifndef SP_UTIL_SORT_SELECTIONSORT_H
#define SP_UTIL_SORT_SELECTIONSORT_H

#include <cstddef>
#include <util/assert.h>
#include <util/comparator.h>

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
selectionsort(T *in, std::size_t length) noexcept {
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

}

#endif
