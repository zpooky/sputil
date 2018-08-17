#ifndef SP_UTIL_SORT_HEAPSORT_H
#define SP_UTIL_SORT_HEAPSORT_H

#include <cstddef>
#include <heap/binary.h>
#include <util/assert.h>
#include <util/comparator.h>

namespace sp {

//=====================================
/*
 * - Inplace sort
 * - No extra memory required
 * - Non-recursive
 *
 * - Not stable
 */
template <typename T, typename Cmp = sp::less>
void
heapsort(T *, std::size_t) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, typename Cmp>
void
heapsort(T *const in, std::size_t length) noexcept {
  if (length == 0) {
    return;
  }
  assertxs(in, length);

  auto heap = heap::heapify<T, sp::inverse<Cmp>>(in, length);

  --heap.length;
  while (heap.length > 0) {
    const std::size_t head = 0;

    using std::swap;
    swap(heap.buffer[head], heap.buffer[heap.length]);

    heap::impl::heap::shift_down(heap, head);
    --heap.length;
  }
}

//=====================================
}
#endif
