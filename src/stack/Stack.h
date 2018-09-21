#ifndef SP_UTIL_STACK_STACK_H
#define SP_UTIL_STACK_STACK_H

#include <util/assert.h>
#include <util/numeric.h>
#include <utility>

namespace sp {
//=====================================
template <typename T>
struct Stack {
  using value_type = T;

  Stack<T> *priv;
  T *buffer;
  std::size_t length;
  std::size_t capacity;

  Stack() noexcept;

  template <std::size_t SIZE>
  explicit Stack(T (&)[SIZE]) noexcept;

  Stack(T *, std::size_t cap) noexcept;

  Stack(T *, std::size_t len, std::size_t cap) noexcept;
};

//=====================================
template <typename T, std::size_t cap>
struct UinStaticStack {
  using value_type = T;
  using storage_type =
      typename std::aligned_storage<sizeof(T) * cap, alignof(T)>::type;
  static constexpr std::size_t capacity = cap;

  UinStaticStack<T, cap> *priv;
  storage_type buffer;
  std::size_t length;

  UinStaticStack() noexcept;

  T *
  data() noexcept;
  const T *
  data() const noexcept;

  T &operator[](std::size_t idx) noexcept;
};

//=====================================
template <typename T>
std::size_t
length(const Stack<T> &) noexcept;

template <typename T, std::size_t cap>
std::size_t
length(const UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T>
std::size_t
capacity(const Stack<T> &) noexcept;

template <typename T, std::size_t cap>
std::size_t
capacity(const UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T>
bool
is_empty(const Stack<T> &) noexcept;

template <typename T, std::size_t cap>
bool
is_empty(const UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T>
bool
is_full(const Stack<T> &) noexcept;

template <typename T, std::size_t cap>
bool
is_full(const UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T, typename V>
T *
push(Stack<T> &, V &&) noexcept;

template <typename T, std::size_t cap, typename V>
T *
push(UinStaticStack<T, cap> &, V &&) noexcept;

//=====================================
template <typename T>
T *
peek(Stack<T> &) noexcept;

template <typename T, std::size_t cap>
T *
peek(UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T>
const T *
peek(const Stack<T> &) noexcept;

template <typename T, std::size_t cap>
const T *
peek(const UinStaticStack<T, cap> &) noexcept;

//=====================================
template <typename T>
bool
pop(Stack<T> &, T &out) noexcept;

template <typename T, std::size_t cap>
bool
pop(UinStaticStack<T, cap> &, T &out) noexcept;

//=====================================
template <typename T, typename F>
void
for_each(Stack<T> &, F) noexcept;

template <typename T, typename F>
void
for_each(const Stack<T> &, F) noexcept;

template <typename T, std::size_t cap, typename F>
void
for_each(UinStaticStack<T, cap> &, F) noexcept;

template <typename T, std::size_t cap, typename F>
void
for_each(const UinStaticStack<T, cap> &, F) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
Stack<T>::Stack() noexcept
    : priv(nullptr)
    , buffer(nullptr)
    , length(0)
    , capacity(0) {
}

template <typename T>
template <std::size_t SIZE>
Stack<T>::Stack(T (&b)[SIZE]) noexcept
    : priv(nullptr)
    , buffer(b)
    , length(0)
    , capacity(SIZE) {
}

template <typename T>
Stack<T>::Stack(T *b, std::size_t cap) noexcept
    : priv(nullptr)
    , buffer(b)
    , length(0)
    , capacity(cap) {
}

template <typename T>
Stack<T>::Stack(T *b, std::size_t len, std::size_t cap) noexcept
    : priv(nullptr)
    , buffer(b)
    , length(len)
    , capacity(cap) {
}

template <typename T, std::size_t cap>
UinStaticStack<T, cap>::UinStaticStack() noexcept
    : priv(nullptr)
    , buffer{}
    , length{0} {
}

//=====================================
template <typename T, std::size_t cap>
T *
UinStaticStack<T, cap>::data() noexcept {
  return reinterpret_cast<T *>(&buffer);
}

template <typename T, std::size_t cap>
const T *
UinStaticStack<T, cap>::data() const noexcept {
  return reinterpret_cast<const T *>(&buffer);
}

template <typename T, std::size_t cap>
T &UinStaticStack<T, cap>::operator[](std::size_t idx) noexcept {
  assertx(idx < length);
  return data()[idx];
}

//=====================================
template <typename T>
std::size_t
length(const Stack<T> &stack) noexcept {
  return stack.length;
}

template <typename T, std::size_t cap>
std::size_t
length(const UinStaticStack<T, cap> &stack) noexcept {
  return stack.length;
}

//=====================================
template <typename T>
std::size_t
capacity(const Stack<T> &stack) noexcept {
  return stack.capacity;
}

template <typename T, std::size_t cap>
std::size_t
capacity(const UinStaticStack<T, cap> &stack) noexcept {
  return stack.capacity;
}

//=====================================
template <typename T>
bool
is_empty(const Stack<T> &stack) noexcept {
  return stack.length == 0;
}

template <typename T, std::size_t cap>
bool
is_empty(const UinStaticStack<T, cap> &stack) noexcept {
  return stack.length == 0;
}

//=====================================
template <typename T>
bool
is_full(const Stack<T> &stack) noexcept {
  return stack.length == capacity;
}

template <typename T, std::size_t cap>
bool
is_full(const UinStaticStack<T, cap> &stack) noexcept {
  return stack.length == stack.capacity;
}

//=====================================
// XXX notexcept if T(val) is noexcept
template <typename T, typename V>
T *
push(Stack<T> &stack, V &&val) //
    noexcept(/*noexcept(T(std::forward<V>(std::declval<V &&>())))*/ true) {
  if (stack.length < stack.capacity) {
    std::size_t idx = stack.length++;
    T *const raw = stack.buffer + idx;
    raw->~T();

    return new (raw) T(std::forward<V>(val));
  }
  return nullptr;
}

template <typename T, std::size_t cap, typename V>
T *
push(UinStaticStack<T, cap> &stack, V &&val) noexcept {
  if (stack.length < stack.capacity) {
    std::size_t idx = stack.length++;
    T *const raw = stack.data() + idx;

    return new (raw) T(std::forward<V>(val));
  }
  return nullptr;
}

//=====================================
template <typename T>
T *
peek(Stack<T> &stack) noexcept {
  const Stack<T> &c_stack = stack;
  return (T *)peek(c_stack);
}

template <typename T, std::size_t cap>
T *
peek(UinStaticStack<T, cap> &stack) noexcept {
  Stack<T> s(stack.data(), stack.length, stack.capacity);
  return peek(s);
}

//=====================================
template <typename T>
const T *
peek(const Stack<T> &stack) noexcept {
  if (stack.length > 0) {
    std::size_t idx = stack.length - 1;
    return stack.buffer + idx;
  }

  return nullptr;
}

template <typename T, std::size_t cap>
const T *
peek(const UinStaticStack<T, cap> &stack) noexcept {
  const Stack<T> s(stack.data(), stack.length, stack.capacity);
  return peek(s);
}

//=====================================
// XXX noexcept if swap() is noexcept
template <typename T>
bool
pop(Stack<T> &stack, T &out) noexcept {
  if (stack.length > 0) {
    std::size_t idx = --stack.length;
    T &instack = stack.buffer[idx];
    using sp::swap;
    swap(instack, out);
    return true;
  }

  return false;
}

template <typename T, std::size_t cap>
bool
pop(UinStaticStack<T, cap> &stack, T &out) noexcept {
  if (stack.length > 0) {
    T &instack = stack[stack.length - 1];
    --stack.length;

    out = std::move(instack);
    instack.~T();
    return true;
  }

  return false;
}

//=====================================
template <typename T, typename F>
void
for_each(Stack<T> &stack, F f) noexcept {
  std::size_t idx = stack.length;
  while (idx > 0) {
    T &current = stack.buffer[--idx];
    f(current);
  }
}

template <typename T, typename F>
void
for_each(const Stack<T> &stack, F f) noexcept {
  std::size_t idx = stack.length;
  while (idx > 0) {
    const T &current = stack.buffer[--idx];
    f(current);
  }
}

template <typename T, std::size_t cap, typename F>
void
for_each(UinStaticStack<T, cap> &stack, F f) noexcept {
  Stack<T> s(stack.data(), stack.length, stack.capacity);
  return for_each(s, f);
}

template <typename T, std::size_t cap, typename F>
void
for_each(const UinStaticStack<T, cap> &stack, F f) noexcept {
  const Stack<T> s((T *)stack.data(), stack.length, stack.capacity);
  return for_each(s, f);
}

//=====================================
} // namespace sp

#endif
