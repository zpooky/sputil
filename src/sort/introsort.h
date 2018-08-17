#ifndef SP_UTIL_SORT_INTROSORT_H
#define SP_UTIL_SORT_INTROSORT_H

#include <cmath>
#include <cstddef>
#include <sort/heapsort.h>
#include <sort/insertionsort.h>
#include <sort/quicksort.h>
#include <util/assert.h>
#include <util/comparator.h>

namespace sp {
namespace rec {

//=====================================
/*
 * https://en.wikipedia.org/wiki/Introsort
 * Is an hybrid sorting algorithm, which means that it uses more than one
 * sorting algorithms
 *
 * quicksort & heapsort is not stable = therefore neither is introsort
 */
template <typename T, typename Cmp = sp::less>
void
introsort(T *, std::size_t) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T, typename Cmp>
static void
introsort(T *const in, std::size_t length, std::size_t depth) noexcept {
  if (length <= 1) {
    return;
  }
  assertxs(in, in, length);

  if (length < 16) {
    // insertionsort
    sp::insertionsort<T, Cmp>(in, length);
  } else if (depth > 0) {
    // quicksort
    std::size_t pivot = sp::rec::quick::partition<T, Cmp>(in, length);
    introsort<T, Cmp>(in, pivot, depth - 1);
    introsort<T, Cmp>(in + pivot, length - pivot, depth - 1);
  } else {
    // heapsort
    sp::heapsort<T, Cmp>(in, length);
  }
}

} // namespace impl

//=====================================
template <typename T, typename Cmp>
void
introsort(T *const in, std::size_t length) noexcept {
  if (length == 0) {
    return;
  }
  assertxs(in, length);

  std::size_t depth = std::log(length) * 2;
  impl::introsort<T, Cmp>(in, length, depth);
}

//=====================================
}
}
#endif
