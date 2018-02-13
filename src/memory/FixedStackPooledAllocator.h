#ifndef SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H

/*
 * replace DynamicFixedLinkedList with an LinkedList using this allocator
 * template<typename T>
 * using DynamicFixedLinkedList =
 * sp::LinkedList<T,sp::FixedStackPooledAllocator>;
 *
 * template<typename T>
 * void ensure_capacity(DynamicFixedLinkedList<T>&,std::size_t);
 */

namespace sp {
template <typename T>
struct FixedStackPooledAllocator {
  std::size_t capacity;
  std::size_t length;

  FixedStackPooledAllocator() noexcept
      : capacity(0)
      , length(0) {
  }
  T *
  allocate(std::size_t) noexcept {
    // TODO
    if (length < capacity) {
      length++;
      return (T *)malloc(sizeof(T));
    }
    return nullptr;
  }

  void
  deallocate(T *ptr, std::size_t) noexcept {
    // TODO
    length--;
    free(ptr);
  }
};

template <typename T>
bool
ensure(FixedStackPooledAllocator<T> &a, std::size_t cap) noexcept {
  // TODO
  a.capacity = cap;
  return true;
}
}

#endif
