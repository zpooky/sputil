#ifndef SP_UTIL_MEMORY_ALLOCATOR_H
#define SP_UTIL_MEMORY_ALLOCATOR_H

#include <cstdlib>

// TODO hide malloc in impl to not require cstdlib include
namespace sp {
template <typename T>
struct Allocator {};

template <typename T>
T *
allocate(Allocator<T> &, std::size_t) noexcept {
  T *result = (T *)malloc(sizeof(T));
  // printf("alloc(%p)\n", result);
  memset(result, 0, sizeof(T));
  return result;
}

template <typename T>
T *
allocate(Allocator<T> &a) noexcept {
  return allocate(a, 1);
}

template <typename T>
void
deallocate(Allocator<T> &, T *ptr, std::size_t) noexcept {
  // printf("dealloc(%p)\n", ptr);
  memset(ptr, 0, sizeof(T));
  free(ptr);
}
template <typename T>
void
deallocate(Allocator<T> &a, T *ptr) noexcept {
  return deallocate(a, ptr, 1);
}

} // namespace sp
#endif
