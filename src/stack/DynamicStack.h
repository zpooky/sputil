#ifndef SP_UTIL_STACK_HEAP_STACK_H
#define SP_UTIL_STACK_HEAP_STACK_H

#include <memory/Allocator.h>
#include <stack/Stack.h>

namespace sp {
//=====================================
template <typename T, template <typename> class Allocator = sp::Allocator>
struct DynamicStack {
  using value_type = T;
  using node_type = UinStaticStack<T, 1024>;

  node_type *head;

  Allocator<node_type> allocator;

  DynamicStack() noexcept;
  DynamicStack(const DynamicStack &) = delete;
  DynamicStack(DynamicStack<T, Allocator> &&) noexcept;

  DynamicStack &
  operator=(const DynamicStack &) = delete;
  DynamicStack &
  operator=(const DynamicStack &&) = delete;

  ~DynamicStack() noexcept;
};

//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const DynamicStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const DynamicStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
is_full(const DynamicStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A, typename V>
T *
push(DynamicStack<T, A> &, V &&) noexcept;

//=====================================
template <typename T, template <typename> class A>
T *
peek(DynamicStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
const T *
peek(const DynamicStack<T, A> &) noexcept;

//=====================================
template <typename T, template <typename> class A>
bool
pop(DynamicStack<T, A> &, T &out) noexcept;

//=====================================
template <typename T, template <typename> class A, template <typename> class A2>
bool
insert(DynamicStack<T, A> &, const DynamicStack<T, A2> &) noexcept;

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(DynamicStack<T, A> &, F) noexcept;

template <typename T, template <typename> class A, typename F>
void
for_each(const DynamicStack<T, A> &, F) noexcept;

//=====================================
template <typename T, template <typename> class A>
void
swap(DynamicStack<T, A> &, DynamicStack<T, A> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, template <typename> class A>
DynamicStack<T, A>::DynamicStack() noexcept
    : head(nullptr)
    , allocator() {
}

template <typename T, template <typename> class A>
DynamicStack<T, A>::DynamicStack(DynamicStack<T, A> &&o) noexcept
    : DynamicStack() {
  using sp::swap;
  swap(head, o.head);
}

template <typename T, template <typename> class A>
DynamicStack<T, A>::~DynamicStack() noexcept {
  node_type *it = head;
  head = nullptr;
  while (it) {
    node_type *priv = it->priv;
    deallocate(allocator, it);
    it = priv;
  }
}

//=====================================
template <typename T, template <typename> class A>
std::size_t
length(const DynamicStack<T, A> &self) noexcept {
  std::size_t result = 0;
  const auto *head = self.head;
  while (head) {
    result += length(*head);
    head = head->priv;
  }
  return result;
}

//=====================================
template <typename T, template <typename> class A>
bool
is_empty(const DynamicStack<T, A> &self) noexcept {
  return self.head == nullptr;
}

//=====================================
template <typename T, template <typename> class A>
bool
is_full(const DynamicStack<T, A> &) noexcept {
  return false;
}

//=====================================
template <typename T, template <typename> class A, typename V>
T *
push(DynamicStack<T, A> &self, V &&value) noexcept {
  if (!self.head || is_full(*self.head)) {
    auto *node = allocate(self.allocator);
    if (!node) {
      return nullptr;
    }
    new (node) UinStaticStack<T, 1024>;
    node->priv = self.head;
    self.head = node;
  }

  return push(*self.head, std::forward<V>(value));
}

//=====================================
template <typename T, template <typename> class A>
T *
peek(DynamicStack<T, A> &self) noexcept {
  if (self.head) {
    return peek(*self.head);
  }

  return nullptr;
}

//=====================================
template <typename T, template <typename> class A>
const T *
peek(const DynamicStack<T, A> &self) noexcept {
  if (self.head) {
    return peek(*self.head);
  }

  return nullptr;
}

//=====================================
template <typename T, template <typename> class A>
bool
pop(DynamicStack<T, A> &self, T &out) noexcept {
  UinStaticStack<T, 1024> *const current = self.head;
  if (current) {
    bool result = pop(*current, out);
    assertx(result);

    if (result && is_empty(*current)) {
      self.head = current->priv;
      deallocate(self.allocator, current);
    }

    return result;
  }

  return false;
}

//=====================================
template <typename T, template <typename> class A, template <typename> class A2>
bool
insert(DynamicStack<T, A> &self, const DynamicStack<T, A2> &from) noexcept {

  for_each(from, [&self](const T &current) {
    /**/
    push(self, current);
  });

  return true;
}

//=====================================
template <typename T, template <typename> class A, typename F>
void
for_each(DynamicStack<T, A> &self, F f) noexcept {
  typename DynamicStack<T, A>::node_type *head = self.head;
  while (head) {
    for_each(*head, f);
    head = head->priv;
  }
}

template <typename T, template <typename> class A, typename F>
void
for_each(const DynamicStack<T, A> &self, F f) noexcept {
  const typename DynamicStack<T, A>::node_type *head = self.head;
  while (head) {
    for_each(*head, f);
    head = head->priv;
  }
}

//=====================================
template <typename T, template <typename> class A>
void
swap(DynamicStack<T, A> &first, DynamicStack<T, A> &second) noexcept {
  using std::swap;
  swap(first.head, second.head);
  swap(first.allocator, second.allocator);
}

//=====================================
} // namespace sp

#endif
