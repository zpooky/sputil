#ifndef SP_UTIL_COLLECTION_ARRAY_H
#define SP_UTIL_COLLECTION_ARRAY_H

#include <cstddef>
#include <util/numeric.h>
#include <utility>

namespace sp {
template <typename T>
struct Array {
  T *buffer;
  std::size_t length;
  std::size_t capacity;

  Array() noexcept;

  template <std::size_t SIZE>
  explicit Array(T (&)[SIZE]) noexcept;

  Array(T *, std::size_t) noexcept;
};

template <typename T, typename V>
T *
insert(Array<T> &, V &&) noexcept;

template <typename T>
T *
get(Array<T> &, std::size_t) noexcept;

template <typename T>
const T *
get(const Array<T> &, std::size_t) noexcept;

template <typename T>
bool
remove(Array<T> &, std::size_t) noexcept;

template <typename T>
bool
take(Array<T> &, std::size_t, T &) noexcept;

template <typename Random, typename T>
void
shuffle(Random &, Array<T> &) noexcept;

template <typename T>
void
swap(Array<T> &, Array<T> &) noexcept;

/*
 * =======================================================
 */

template <typename T>
Array<T>::Array() noexcept
    : Array(nullptr, 0) {
}

template <typename T>
template <std::size_t SIZE>
Array<T>::Array(T (&b)[SIZE]) noexcept
    : Array(b, SIZE) {
}

template <typename T>
Array<T>::Array(T *b, std::size_t size) noexcept
    : buffer(b)
    , length(0)
    , capacity(size) {
}

template <typename T, typename V>
T *
insert(Array<T> &a, V &&val) noexcept {
  if (a.length < a.capacity) {
    std::size_t idx = a.length++;
    T *const raw = a.buffer + idx;
    raw->~T();
    new (raw) T(std::forward<V>(val));
    return raw;
  }

  return nullptr;
}

template <typename T>
T *
get(Array<T> &a, std::size_t idx) noexcept {
  const Array<T> &c_a = a;
  return (T *)get(c_a, idx);
}

template <typename T>
const T *
get(const Array<T> &a, std::size_t idx) noexcept {
  if (idx < a.length) {
    return &a.buffer[idx];
  }
  return nullptr;
}

template <typename T>
bool
remove(Array<T> &a, std::size_t idx) noexcept {
  if (idx < a.length) {
    std::size_t last = --a.length;
    if (idx != last) {
      using sp::swap;
      swap(a.buffer[idx], a.buffer[last]);
    }

    T *const raw = a.buffer + last;
    raw->~T();
    new (raw) T;
    return true;
  }

  return false;
}

template <typename T>
bool
take(Array<T> &a, std::size_t idx, T &out) noexcept {
  if (idx < a.length) {
    using sp::swap;
    swap(out, a.buffer[idx]);

    assert(remove(a, idx));
    return true;
  }
  return false;
}

template <typename Random, typename T>
void
shuffle(Random &r, Array<T> &a) noexcept {
  for (std::size_t idx = 0; idx < a.length; ++idx) {
    auto replace = uniform_dist(r, std::uint32_t(0), std::uint32_t(a.length));
    if (idx != replace) {
      using sp::swap;
      swap(a.buffer[idx], a.buffer[replace]);
    }
  }
}

template <typename T>
void
swap(Array<T> &f, Array<T> &s) noexcept {
  swap(f.buffer, s.buffer);
  swap(f.length, s.length);
  swap(f.capacity, s.capacity);
}

} // namespace sp

#endif
