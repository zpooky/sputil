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
template <typename T>
static const T *
pivot(const T *in, std::size_t length) noexcept {
  assertx(length > 0);
  assertx(in);
  return in + (length - 1);
  // return in;//TODO verify this
}

// A function that find the middle of the
// values pointed by the pointers a, b, c
// and return that pointer
template <typename T>
static const T *
median_of_three(const T *in, std::size_t length) {
  if (length <= 2) {
    return pivot(in, length);
  }

  const T *const beg = in;
  const T *const mid = in + (length / 2);
  const T *const lst = in + (length - 1);

  if (*beg < *mid && *mid < *lst) {
    return mid;
  }

  if (*beg < *lst && *lst <= *mid) {
    return lst;
  }

  if (*mid <= *beg && *beg < *lst) {
    return beg;
  }

  if (*mid < *lst && *lst <= *beg) {
    return lst;
  }

  if (*lst <= *beg && *beg < *mid) {
    return beg;
  }

  // if (*lst <= *mid && *mid <= *lst) {
  return mid;
  // }
}

/* Hoare partition scheme */
template <typename T, typename C>
static std::size_t
partition(T *const in, std::size_t length) noexcept {
  T *head = in;
  T *tail = in + (length - 1);

  const T *const start = in;
  const T *const end = in + length;

  const T *p = median_of_three<T>(in, length);

  C cmp;
  while (true) {
    while (head != end && cmp(*head, /*<*/ *p)) {
      // aka: head < pivot
      head++;
    }
    while (tail != start && cmp(*p, /*<*/ *tail)) {
      // aka: tail > pivot
      tail--;
    }

    if (head >= tail) {
      break;
    }

    assertx(head < end);
    assertx(tail > start);

    /* Pivot needs to be the same even if its position gets swapped. */
    if (head == p) {
      p = tail;
    } else if (tail == p) {
      p = head;
    }

    using std::swap;
    swap(*head++, *tail--);
  }

  assertx(head < end);
  assertx(head >= start);

  assertx(tail < end);
  assertx(tail >= start);

  return std::size_t(head - in);
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
