#ifndef SP_UTIL_QUEUE_QUEUE_H
#define SP_UTIL_QUEUE_QUEUE_H

#include <list/LinkedList.h>
#include <memory/Allocator.h>

namespace sp {
template <typename T, template <typename> class Allocator = sp::Allocator>
struct LinkedListQueue {
  /**/
  LinkedList<T, Allocator> impl;
  LinkedListQueue() noexcept;

  LinkedListQueue(const LinkedListQueue &) = delete;
  LinkedListQueue(LinkedListQueue &&) noexcept;

  LinkedListQueue &
  operator=(const LinkedListQueue &) = delete;
  LinkedListQueue &
  operator=(const LinkedListQueue &&) = delete;
};

//=====================================
template <typename T, template <typename> class A, typename V>
T *
enqueue(LinkedListQueue<T, A> &, V &&) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
dequeue(LinkedListQueue<T, A> &, T &out) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const LinkedListQueue<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const LinkedListQueue<T, A> &) noexcept;

//=====================================

/*
 * =======================================================
 */
template <typename T, template <typename> class A>
LinkedListQueue<T, A>::LinkedListQueue() noexcept
    : impl() {
}

template <typename T, template <typename> class A>
LinkedListQueue<T, A>::LinkedListQueue(LinkedListQueue &&o) noexcept
    : impl(std::move(o.impl)) {
}

//=====================================
template <typename T, template <typename> class A, typename V>
T *
enqueue(LinkedListQueue<T, A> &queue, V &&val) noexcept {
  return push_back(queue.impl, std::forward<V>(val));
}
//=====================================
template <typename T, template <typename> class A>
bool
dequeue(LinkedListQueue<T, A> &queue, T &out) noexcept {
  T *head = get(queue.impl, 0);
  if (head) {
    // using sp::swap;
    using std::swap;
    swap(*head, out);

    auto res = remove_first(queue.impl, [](const auto &) {
      /**/
      return true;
    });
    assertx(res);

    return res;
  }

  return false;
}
//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const LinkedListQueue<T, A> &queue) noexcept {
  return is_empty(queue.impl);
}
//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const LinkedListQueue<T, A> &queue) noexcept {
  return queue.length;
}

//=====================================
} // namespace sp

#endif
