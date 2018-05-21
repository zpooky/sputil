#ifndef SP_UTIL_SORT_QUICKSORT_H
#define SP_UTIL_SORT_QUICKSORT_H

#include <cstddef>
#include <util/assert.h>
// #include <util/comparator.h>
// TODO use comparator

namespace sp {
namespace rec {

//=====================================
template <typename T>
void
quicksort(T *, std::size_t) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {

/*
 * Hoare partition scheme
 */
template <typename T>
std::size_t
partition(T *arr, std::size_t length) noexcept {
  std::size_t head = 0;
  std::size_t tail = length - 1;

  std::size_t pivot = tail;

  while (true) {
    while (head < length && (arr[head] < arr[pivot])) {
      head++;
    }
    while (tail > 0 && (arr[tail] > arr[pivot])) {
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

    std::swap(arr[head], arr[tail]);
    head++;
    tail--;
  }

  return head;
}
} // namespace impl
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
template <typename T>
void
quicksort(T *arr, std::size_t length) noexcept {
  if (length > 1) {
    assertx(arr);

    std::size_t pivot = impl::partition(arr, length);
    quicksort(arr, pivot);
    quicksort(arr + pivot, length - pivot);
  }
}

//=====================================

} // namespace rec
} // namespace sp

#endif
