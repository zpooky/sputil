#ifndef SP_UTIL_MEMORY_ALLOCATOR_H
#define SP_UTIL_MEMORY_ALLOCATOR_H

#include <cstdlib>
#include <cstring>
#include <util/assert.h>

// TODO hide malloc in impl to not require cstdlib include
namespace sp {
//=====================================
template <typename T>
struct Allocator {};

//=====================================
template <typename T>
T *
allocate(Allocator<T> &, std::size_t) noexcept {
  void *const result = malloc(sizeof(T));
  // printf("alloc(%p)\n", result);
  std::memset(result, 0, sizeof(T));
  return (T *)result;
}

template <typename T>
T *
allocate(Allocator<T> &a) noexcept {
  return allocate(a, 1);
}

//=====================================
template <typename T>
void
deallocate(Allocator<T> &, T *ptr, std::size_t) noexcept {
  assertx(ptr);
  // printf("dealloc(%p)\n", ptr);
  void *v = ptr;
  std::memset(v, 0, sizeof(T));
  free(ptr);
}

template <typename T>
void
deallocate(Allocator<T> &a, T *ptr) noexcept {
  return deallocate(a, ptr, 1);
}

//=====================================
template <typename T>
void
swap(Allocator<T> &, Allocator<T> &) noexcept {
}

} // namespace sp

#endif
