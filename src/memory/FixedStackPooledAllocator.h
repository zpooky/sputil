#ifndef SP_UTIL_MEMORY_FIXED_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_FIXED_STACK_POOLED_ALLOCATOR_H

#include <memory/StackPooledAllocator.h>

namespace sp {
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

template <typename T>
bool
ensure(FixedStackPooledAllocator<T> &a, std::size_t cap) noexcept {
  // TODO
  a.capacity = cap;
  return true;
}

template <typename T>
T *
allocate(FixedStackPooledAllocator<T> &a) noexcept {
  if (a.length < a.capacity) {
    a.length++;
    return allocate(a.allocator);
  }
  return nullptr;
}

template <typename T>
void
deallocate(FixedStackPooledAllocator<T> &a, T *ptr) noexcept {
  assert(a.length > 0);
  a.length--;
  return deallocate(a.allocator, ptr);
}
}

#endif
