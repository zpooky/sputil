#ifndef SP_UTIL_MEMORY_FIXED_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_FIXED_STACK_POOLED_ALLOCATOR_H

#include <cstring>
#include <memory/StackPooledAllocator.h>
#include <util/assert.h>

namespace sp {
//=====================================
template <typename T>
struct FixedStackPooledAllocator {
  StackPooledAllocator<T> allocator;
  std::size_t capacity;
  std::size_t length;

  FixedStackPooledAllocator() noexcept
      : allocator()
      , capacity(0)
      , length(0) {
  }
};

//=====================================
template <typename T>
bool
ensure(FixedStackPooledAllocator<T> &a, std::size_t cap) noexcept {
  // TODO
  a.capacity = cap;
  return true;
}

//=====================================
template <typename T>
T *
allocate(FixedStackPooledAllocator<T> &a) noexcept {
  if (a.length < a.capacity) {
    a.length++;
    T *const result = allocate(a.allocator);

    void *const p = result;
    std::memset(p, 0, sizeof(T));

    // printf("alloc(%p)\n", result);
    return result;
  }

  return nullptr;
}

//=====================================
template <typename T>
void
deallocate(FixedStackPooledAllocator<T> &a, T *ptr) noexcept {
  assertx(a.length > 0);
  a.length--;

  // printf("dealloc(%p)\n", ptr);
  return deallocate(a.allocator, ptr);
}

//=====================================
template <typename T>
void
swap(FixedStackPooledAllocator<T> &f,
     FixedStackPooledAllocator<T> &s) noexcept {
  using std::swap;

  swap(f.allocator, s.second);
  swap(f.capacity, s.capacity);
  swap(f.length, s.length);
}

//=====================================
} // namespace sp

#endif
