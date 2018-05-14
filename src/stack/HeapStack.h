#ifndef SP_UTIL_STACK_HEAP_STACK_H
#define SP_UTIL_STACK_HEAP_STACK_H

#include <memory/Allocator.h>
#include <stack/Stack.h>

namespace sp {

//=====================================
template <typename T, template <typename> class Allocator = sp::Allocator>
struct HeapStack {
  using value_type = T;
  using node_type = UinStaticStack<T, 1024>;
  node_type *head;

  Allocator<node_type> allocator;

  HeapStack() noexcept;
  HeapStack(const HeapStack &) = delete;
  HeapStack(HeapStack<T, Allocator> &&) noexcept;

  HeapStack &
  operator=(const HeapStack &) = delete;
  HeapStack &
  operator=(const HeapStack &&) = delete;

  ~HeapStack() noexcept;
};

//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const HeapStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const HeapStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_full(const HeapStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A, typename V>
T *
push(HeapStack<T, A> &, V &&) noexcept;

//=====================================
template <typename T, template <typename> class A>
T *
peek(HeapStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
const T *
peek(const HeapStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
pop(HeapStack<T, A> &, T &out) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(HeapStack<T, A> &, F) noexcept;

template <typename T, template <typename> class A, typename F>
void
for_each(const HeapStack<T, A> &, F) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, template <typename> class A>
HeapStack<T, A>::HeapStack() noexcept
    : head(nullptr)
    , allocator() {
}

template <typename T, template <typename> class A>
HeapStack<T, A>::HeapStack(HeapStack<T, A> &&o) noexcept
    : HeapStack() {
  using sp::swap;
  swap(head, o.head);
}

template <typename T, template <typename> class A>
HeapStack<T, A>::~HeapStack() noexcept {
  // TODO
}

//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const HeapStack<T, A> &stack) noexcept {
  std::size_t result = 0;
  const auto *head = stack.head;
  while (head) {
    result += length(*head);
    head = head->priv;
  }
  return result;
}
//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const HeapStack<T, A> &stack) noexcept {
  return stack.head == nullptr;
}
//=====================================
template <typename T, template <typename> class A>
bool
is_full(const HeapStack<T, A> &) noexcept {
  return false;
}
//=====================================
template <typename T, template <typename> class A, typename V>
T *
push(HeapStack<T, A> &stack, V &&value) noexcept {
  if (!stack.head || is_full(*stack.head)) {
    auto &allocator = stack.allocator;
    auto *node = allocate(allocator);
    if (!node) {
      return nullptr;
    }
    new (node) UinStaticStack<T, 1024>;
    node->priv = stack.head;
    stack.head = node;
  }

  return push(*stack.head, std::forward<V>(value));
}
//=====================================
template <typename T, template <typename> class A>
T *
peek(HeapStack<T, A> &stack) noexcept {
  if (stack.head) {
    return peek(*stack.head);
  }
  return nullptr;
}
//=====================================
template <typename T, template <typename> class A>
const T *
peek(const HeapStack<T, A> &stack) noexcept {
  if (stack.head) {
    return peek(*stack.head);
  }
  return nullptr;
}
//=====================================
template <typename T, template <typename> class A>
bool
pop(HeapStack<T, A> &stack, T &out) noexcept {
  UinStaticStack<T, 1024> *const current = stack.head;
  if (current) {
    bool result = pop(*current, out);
    assertx(result);

    if (result && is_empty(*current)) {
      stack.head = current->priv;
      deallocate(stack.allocator, current);
    }

    return result;
  }

  return false;
}
//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(HeapStack<T, A> &stack, F f) noexcept {
  auto *head = stack.head;
  while (head) {
    for_each(*head, f);
    head = head->priv;
  }
}

template <typename T, template <typename> class A, typename F>
void
for_each(const HeapStack<T, A> &stack, F f) noexcept {
  const auto *head = stack.head;
  while (head) {
    for_each(*head, f);
    head = head->priv;
  }
}

//=====================================
}

#endif
