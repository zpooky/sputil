#ifndef SP_UTIL_LIST_FIXED_LIST_H
#define SP_UTIL_LIST_FIXED_LIST_H

#include <list/LinkedList.h>
#include <memory/FixedStackPooledAllocator.h>

namespace sp {
template <typename T>
using FixedList = LinkedList<T, sp::FixedStackPooledAllocator>;

template <typename T>
bool
init(FixedList<T> &, std::size_t) noexcept;

template <typename T>
std::size_t
length(const FixedList<T> &) noexcept;

template <typename T>
std::size_t
capacity(const FixedList<T> &) noexcept;

/*
 * =======================================================
 */

template <typename T>
bool
init(FixedList<T> &l, std::size_t capacity) noexcept {
  auto &a = l.allocator;
  return ensure(a, capacity);
} // sp::init()

template <typename T>
std::size_t
length(const FixedList<T> &l) noexcept {
  auto &a = l.allocator;
  return a.length;
}

template <typename T>
std::size_t
capacity(const FixedList<T> &l) noexcept {
  auto &a = l.allocator;
  return a.capacity;
}

} // namespace sp

#endif
