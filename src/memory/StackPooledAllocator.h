#ifndef SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H
#define SP_UTIL_MEMORY_STACK_POOLED_ALLOCATOR_H

#include <cstring>
#include <memory/Allocator.h>
#include <util/assert.h>

// TODO hide impl detail only pass down size and alignment

/*
 * XXX only memset 0 in debug mode
 */

namespace sp {
template <typename T>
struct StackPooledAllocator {
  using value_type = T;

  static_assert(sizeof(T) >= sizeof(void *), "");
  static_assert(alignof(T) % alignof(void *) == 0, "");

  void *stack;

  StackPooledAllocator() noexcept;
  ~StackPooledAllocator() noexcept;

  StackPooledAllocator(const StackPooledAllocator<T> &&) = delete;

  StackPooledAllocator<T> &
  operator=(const StackPooledAllocator<T> &) = delete;
  StackPooledAllocator<T> &
  operator=(const StackPooledAllocator<T> &&) = delete;
};

template <typename T>
T *
allocate(StackPooledAllocator<T> &) noexcept;

template <typename T>
void
deallocate(StackPooledAllocator<T> &, T *) noexcept;

/*
 * ==========================================================================
 */

namespace impl {
namespace StackPooledAllocator {
struct SPANode {
  SPANode *next;
  explicit SPANode(SPANode *n) noexcept
      : next(n) {
  }
};

static inline SPANode *
to_node(void *in) noexcept {
  if (in) {
    assertx(reinterpret_cast<uintptr_t>(in) % alignof(void *) == 0);
  }

  return (SPANode *)in;
}
} // namespace StackPooledAllocator
} // namespace impl

template <typename T>
StackPooledAllocator<T>::StackPooledAllocator() noexcept
    : stack(nullptr) {
}

template <typename T>
StackPooledAllocator<T>::~StackPooledAllocator() noexcept {
  using namespace impl::StackPooledAllocator;
  SPANode *it = to_node(stack);
Lit:
  if (it) {
    SPANode *next = it->next;
    std::free(it);

    it = next;
    goto Lit;
  }
  stack = nullptr;
}

template <typename T>
T *
allocate(StackPooledAllocator<T> &a) noexcept {
  using namespace impl::StackPooledAllocator;

  if (a.stack) {
    SPANode *result = to_node(a.stack);
    a.stack = result->next;

    void *const p = result;
    std::memset(p, 0, sizeof(T));
    return reinterpret_cast<T *>(result);
  } else {
    // printf("allocate(%zu, sizeof[%zu], alignof[%zu])\n", n, sizeof(T),
    //        alignof(T));
    void *mem = std::malloc(sizeof(T));
    if (mem) {
      return static_cast<T *>(mem);
    }
  }
  return nullptr;
}

template <typename T>
void
deallocate(StackPooledAllocator<T> &a, T *p) noexcept {
  using namespace impl::StackPooledAllocator;
  void *const pah = p;
  std::memset(pah, 0, sizeof(T));
  a.stack = new (p) SPANode(to_node(a.stack));
}
} // namespace sp

#endif
