#ifndef SP_UTIL_SORT_QUICKSORT_H
#define SP_UTIL_SORT_QUICKSORT_H

#include <cstddef>
#include <util/assert.h>
#include <util/comparator.h>

namespace sp {
namespace rec {
/*
 * TODO median of three pivot selection to be used be the partition algh
 * https://www.geeksforgeeks.org/know-your-sorting-algorithm-set-2-introsort-cs-sorting-weapon/
 */

//=====================================
template <typename T, typename C = sp::less>
void
quicksort(T *, std::size_t) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace quick {

/*
 * Hoare partition scheme
 */
template <typename T, typename C>
std::size_t
partition(T *in, std::size_t length) noexcept {
  std::size_t head = 0;
  std::size_t tail = length - 1;

  std::size_t pivot = tail;

  C cmp;
  while (true) {
    while (head < length && cmp(in[head], /*<*/ in[pivot])) {
      // aka: head < pivot
      head++;
    }
    while (tail > 0 && cmp(in[pivot], /*<*/ in[tail])) {
      // aka: tail > pivot
      tail--;
    }

    if (head >= tail) {
      break;
    }

    assertx(head < length);
    assertx(tail < length);
    assertx(tail > 0);

    /*
     * Pivot needs to be the same even if its position gets swapped.
     */
    if (head == pivot) {
      pivot = tail;
    } else if (tail == pivot) {
      pivot = head;
    }

    using std::swap;
    swap(in[head], in[tail]);

    head++;
    tail--;
  }

  return head;
}
} // namespace quick
//=====================================
/*
 * 1. Pick an element, called a pivot, from the array.
 * 2. Partitioning: reorder the array so that all elements with values less than
 *    the pivot come before the pivot, while all elements with values greater
 *    than the pivot come after it (equal values can go either way). After this
 *    partitioning, the pivot is in its final position. This is called the
 *    partition operation.
 * 3. Recursively apply the above steps to the sub-array of elements with
 *    smaller values and separately to the sub-array of elements with greater
 *    values.
 */
template <typename T, typename C>
void
quicksort(T *in, std::size_t length) noexcept {
  if (length > 1) {
    assertx(in);

    std::size_t pivot = quick::partition<T, C>(in, length);
    quicksort<T, C>(in, pivot);
    quicksort<T, C>(in + pivot, length - pivot);
  }
}

//=====================================

} // namespace rec
} // namespace sp

#endif
