#ifndef SP_UTIL_SORT_QUICKSORT_H
#define SP_UTIL_SORT_QUICKSORT_H

#include <cstddef>

namespace sp {
namespace rec {

template <typename T>
void
quicksort(T *, std::size_t) noexcept;
//=====================================

template <typename T>
void
quicksort(T *arr, std::size_t length) noexcept {
  return;
}
//=====================================

} // namespace rec
} // namespace sp

#endif
