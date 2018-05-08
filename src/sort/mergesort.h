#ifndef SP_UTIL_SORT_MERGESORT_H
#define SP_UTIL_SORT_MERGESORT_H

#include <cstddef>
#include <util/assert.h>
// #include <util/comparator.h>
// TODO use comparator

namespace sp {
namespace rec {
//=====================================
template <typename T>
void
mergesort(T *, std::size_t) noexcept;

//=====================================
namespace impl {
#if 0
template <typename T>
void
shift_before(T *to, T *from) noexcept {
  while (to != from) {
    using std::swap;
    swap(*to, *from);
    ++to;
  }
}
/*
 * everything to the left of /pivot/ is sorted one way
 * everything to the right including /pivot/ is sorted separately one way
 * the function should turn the arr into a completely sorted sequence from
 * 0-length
 */
template <typename T>
void
sp_msort(T *arr, std::size_t length, std::size_t pivot) noexcept {
  // TODO this is probably not mergesort but maybe usefull anyway
  T *fit = arr;
  T *pit = arr + pivot;
  const T *const lend = arr + length;

  const T *end = arr + (length - pivot);
  while (fit != end && pit != lend) {
    if (*fit > *pit) {
      shift_before(fit, pit);

      ++end;
      ++pit;
    }
    ++fit;
  }
}
#endif

#if 0
template <typename T>
void
shift_before(T *front, T *back, const T *end) noexcept {
  assertx(front != end);
  assertx(back != end);

  using std::swap;
  swap(*front, *back);

  T *next = back + 1;
  while (next != end) {
    if (*back > *next) {
      swap(*back, *next);
      ++back;
      ++next;
    } else {
      return;
    }
  }
}

template <typename T>
void
sp_msort(T *arr, std::size_t length, std::size_t pivot) noexcept {
  T *it = arr;
  T *pit = arr + pivot;
  const T *const lend = arr + length;

  const T *end = arr + (length - pivot);
  while (it != end) {
    if (*it > *pit) {
      shift_before(it, pit, lend);
    }
    ++it;
  }
}
#endif
// http://rosettacode.org/wiki/Sorting_algorithms/Merge_sort#C

template <typename T>
void
sp_msort(T *arr, std::size_t length, std::size_t pivot) noexcept {
  assertxs(length > pivot, length, pivot);

  std::size_t first_length = pivot;
  T *first = new T[first_length];
  for (std::size_t i = 0; i < first_length; ++i) {
    first[i] = arr[i];
  }

  T *second = arr + first_length;
  std::size_t second_length(length - first_length);

  std::size_t dest_idx = 0;
  std::size_t f_idx = 0;
  std::size_t s_idx = 0;

  for (std::size_t i = 0; i < std::min(first_length, second_length); ++i) {
    // use '=' here for stable sort
    if (first[f_idx] <= second[s_idx]) {
      arr[dest_idx++] = first[f_idx++];
    } else {
      arr[dest_idx++] = second[s_idx++];
    }
  }

  while (f_idx < first_length) {
    arr[dest_idx++] = first[f_idx++];
  }

  while (s_idx < second_length) {
    arr[dest_idx++] = second[s_idx++];
  }

  assertxs(dest_idx == length, dest_idx, length);

  delete[] first;
}

} // namespace impl
//=====================================

/*
 * 1. If it is only one element in the list it is already sorted, return.
 * 2. Divide the list recursively into two halves until it can no more be
 *    divided.
 * 3. Merge the smaller lists into new list in sorted order.
 */
template <typename T>
void
mergesort(T *arr, std::size_t length) noexcept {
  if (length > 1) {
    std::size_t pivot = length / 2;
    mergesort(arr, pivot);
    mergesort(arr + pivot, length - pivot);
    impl::sp_msort(arr, length, pivot);
  }
}

#if 0
inline void
merge(int arr[], int l, int m, int r) {
  int i, j, k;
  int n1 = m - l + 1;
  int n2 = r - m;

  /* create temp arrays */
  int L[n1], R[n2];

  /* Copy data to temp arrays L[] and R[] */
  for (i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  /* Merge the temp arrays back into arr[l..r]*/
  i = 0; // Initial index of first subarray
  j = 0; // Initial index of second subarray
  k = l; // Initial index of merged subarray
  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  /* Copy the remaining elements of L[], if there
     are any */
  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  /* Copy the remaining elements of R[], if there
     are any */
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
inline void
mergeSort(int arr[], int l, int r) {
  if (l < r) {
    // Same as (l+r)/2, but avoids overflow for
    // large l and h
    int m = l + (r - l) / 2;

    // Sort first and second halves
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);

    merge(arr, l, m, r);
  }
}

inline void
mergeSort(int arr[], int length) {
  if (length > 1) {
    mergeSort(arr, 0, length - 1);
  }
}
#endif

//=====================================
}
}

#endif
