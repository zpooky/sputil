#ifndef SP_UTIL_MEMORY_ALLOCATOR_H
#define SP_UTIL_MEMORY_ALLOCATOR_H

#include <cstdlib>

namespace sp {
template <typename T>
struct Allocator {

  T *
  allocate(std::size_t) noexcept {
    return (T *)malloc(sizeof(T));
  }

  void
  deallocate(T *ptr, std::size_t) noexcept {
    free(ptr);
  }
};
} // namespace sp
#endif
