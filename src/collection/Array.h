#ifndef SP_UTIL_COLLECTION_ARRAY_H
#define SP_UTIL_COLLECTION_ARRAY_H

#include <cstddef>
#include <cstdint>
#include <util/comparator.h>
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
  Array(T *, std::size_t, std::size_t) noexcept;
};

/*
 * Reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct StaticArray : public Array<T> {
  T raw[cap];
  static constexpr std::size_t storage_capacity = cap;

  StaticArray() noexcept;
};

/*
 * Do not reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct UinStaticArray {
  using storage_type =
      typename std::aligned_storage<sizeof(T) * cap, alignof(T)>::type;
  static constexpr std::size_t storage_capacity = cap;

  storage_type buffer;
  std::size_t length;

  UinStaticArray() noexcept;

  constexpr T *
  data() noexcept {
    return reinterpret_cast<T *>(&buffer);
  }
};

template <typename T, typename V>
T *
insert(Array<T> &, V &&) noexcept;

template <typename T, std::size_t c, typename V>
T *
insert(UinStaticArray<T, c> &, V &&) noexcept;

template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert(Array<T> &, V &&) noexcept;

template <typename T, std::size_t c, typename V,
          typename Comparator = sp::greater>
T *
bin_insert(UinStaticArray<T, c> &, V &&) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
T *
bin_search(Array<T> &, const K &) noexcept;

template <typename T, std::size_t c, typename K,
          typename Comparator = sp::greater>
T *
bin_search(UinStaticArray<T, c> &, const K &) noexcept;

template <typename T>
T *
get(Array<T> &, std::size_t) noexcept;

template <typename T, std::size_t c>
T *
get(UinStaticArray<T, c> &, std::size_t) noexcept;

template <typename T>
const T *
get(const Array<T> &, std::size_t) noexcept;

template <typename T, std::size_t c>
const T *
get(const UinStaticArray<T, c> &, std::size_t) noexcept;

template <typename T, typename V>
T *
set(Array<T> &, std::size_t, V &&) noexcept;

template <typename T, std::size_t c, typename V>
T *
set(UinStaticArray<T, c> &, std::size_t, V &&) noexcept;

template <typename T, typename V>
T *
exchange(Array<T> &, std::size_t, /*IN/OUT*/ V &) noexcept;

template <typename T, std::size_t c, typename V>
T *
exchange(UinStaticArray<T, c> &, std::size_t, /*IN/OUT*/ V &) noexcept;

template <typename T>
bool
remove(Array<T> &, std::size_t) noexcept;

template <typename T, std::size_t c>
bool
remove(UinStaticArray<T, c> &, std::size_t) noexcept;

/*
 * Take element at $index and swap it with $out, then swap element at $index
 * again with the last element in the Array and decrement $length.
 */
template <typename T>
bool
take(Array<T> &, std::size_t, /*OUT*/ T &) noexcept;

template <typename T, std::size_t c>
bool
take(UinStaticArray<T, c> &, std::size_t, /*OUT*/ T &) noexcept;

template <typename Random, typename T>
void
shuffle(Random &, Array<T> &) noexcept;

template <typename Random, typename T, std::size_t c>
void
shuffle(Random &, UinStaticArray<T, c> &) noexcept;

template <typename T>
void
clear(Array<T> &) noexcept;

template <typename T, std::size_t c>
void
clear(UinStaticArray<T, c> &) noexcept;

template <typename T>
void
swap(Array<T> &, Array<T> &) noexcept;

template <typename T, typename F>
void
for_each(Array<T> &, F) noexcept;

template <typename T, typename F>
void
for_each(const Array<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(Array<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(const Array<T> &, F) noexcept;

template <typename T, typename Predicate>
T *
find(Array<T> &, Predicate) noexcept;

template <typename T, std::size_t c, typename Predicate>
T *
find(UinStaticArray<T, c> &, Predicate) noexcept;

template <typename T, typename Predicate>
const T *
find(const Array<T> &, Predicate) noexcept;

template <typename T, std::size_t c, typename Predicate>
T *
find(const UinStaticArray<T, c> &, Predicate) noexcept;

template <typename T, typename K, typename F>
K *
reduce(const Array<T> &, K *, F) noexcept;

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

template <typename T>
Array<T>::Array(T *b, std::size_t l, std::size_t size) noexcept
    : buffer(b)
    , length(l)
    , capacity(size) {
}

template <typename T, std::size_t c>
StaticArray<T, c>::StaticArray() noexcept
    : Array<T>(raw)
    , raw{} {
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray() noexcept
    : buffer{}
    , length{0} {
}

template <typename T, typename V>
T *
insert(Array<T> &a, V &&val) noexcept {
  if (a.length < a.capacity) {
    std::size_t idx = a.length++;
    T *const raw = a.buffer + idx;

    // dtor of default init T
    raw->~T();

    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
insert(UinStaticArray<T, c> &a, V &&val) noexcept {
  if (a.length < c) {
    std::size_t idx = a.length++;
    T *const raw = a.data() + idx;

    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert(Array<T> &, V &&) noexcept {
  // TODO
  return nullptr;
}

template <typename T, std::size_t c, typename V,
          typename Comparator = sp::greater>
T *
bin_insert(UinStaticArray<T, c> &a, V &&val) noexcept {
  auto find_successor = [&a, &val]() {
  };
  // TODO
  return nullptr;
}

template <typename T, typename K, typename Comparator = sp::greater>
T *
bin_search(Array<T> &a, const K &needle) noexcept {
  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  if (a.length > 0) {

    std::size_t length = a.length;
    T *p = a.buffer;
  Lit:
    /*
     * p            mid         length
     * |____________|___________|
     * think recursively
     */
    std::size_t mid = middle(length);
    Comparator cmp;
    if /*mid > needle*/ (cmp(p[mid], needle)) {
    } /* mid < needle*/ else if (cmp(needle, p[mid])) {
      p = p + mid;
    } else {
      /* match */
      return p + mid;
    }
    /*
     * Since we have split in half the possible locations of $needle
     */
    length -= mid;

    if (mid != 0) {
      goto Lit;
    }
  }

  return nullptr;
}

template <typename T, std::size_t c, typename K, typename Comparator>
T *
bin_search(UinStaticArray<T, c> &a, const K &needle) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::storage_capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return bin_search<T, K, Comparator>(c_a, needle);
}

template <typename T>
T *
get(Array<T> &a, std::size_t idx) noexcept {
  const auto &c_a = a;
  return (T *)get(c_a, idx);
}

template <typename T, std::size_t c>
T *
get(UinStaticArray<T, c> &a, std::size_t idx) noexcept {
  Array<T> c_a(a.data(), a.length, c);
  return get(c_a, idx);
}

template <typename T>
const T *
get(const Array<T> &a, std::size_t idx) noexcept {
  if (idx < a.length) {
    return &a.buffer[idx];
  }
  return nullptr;
}

template <typename T, std::size_t c>
T *
get(const UinStaticArray<T, c> &a, std::size_t idx) noexcept {
  const auto &c_a = a;
  return get(c_a, idx);
}

template <typename T, typename V>
T *
set(Array<T> &a, std::size_t idx, V &&val) noexcept {
  if (idx < a.length) {
    T *const p = a.buffer + idx;
    p->~T();
    return ::new (p) T(std::forward<V>(val));
  }
  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
set(UinStaticArray<T, c> &a, std::size_t idx, V &&val) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::storage_capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return set(c_a, idx, std::forward<V>(val));
}

template <typename T, typename V>
T *
exchange(Array<T> &a, std::size_t idx, /*IN/OUT*/ V &out) noexcept {
  if (idx < a.length) {
    using sp::swap;
    swap(a.biffer[idx], out);
    return a.buffer + idx;
  }
  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
exchange(UinStaticArray<T, c> &a, std::size_t idx, /*IN/OUT*/ V &out) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::storage_capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return exchange(c_a, idx, out);
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

template <typename T, std::size_t c>
bool
remove(UinStaticArray<T, c> &a, std::size_t idx) noexcept {
  /*
   * Differing only in that we do not defalt construct the empty element
   */
  if (idx < a.length) {
    std::size_t last = --a.length;
    if (idx != last) {
      using sp::swap;
      swap(a.data()[idx], a.data()[last]);
    }

    T *const raw = a.buffer + last;
    raw->~T();

    return true;
  }

  return false;
}

template <typename T>
bool
take(Array<T> &a, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < a.length) {
    using sp::swap;
    swap(out, a.buffer[idx]);

    assert(remove(a, idx));
    return true;
  }
  return false;
}

template <typename T, std::size_t c>
bool
take(UinStaticArray<T, c> &a, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < a.length) {
    using sp::swap;
    swap(out, a.data()[idx]);

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

template <typename Random, typename T, std::size_t c>
void
shuffle(Random &r, UinStaticArray<T, c> &a) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::storage_capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return shuffle(r, c_a);
}

template <typename T>
void
clear(Array<T> &a) noexcept {
  for (std::size_t idx = 0; idx < a.length; ++idx) {
    T *const p = a.buffer + idx;
    p->~T();
    new (p) T();
  }
  a.length = 0;
}

template <typename T, std::size_t c>
void
clear(UinStaticArray<T, c> &a) noexcept {
  for (std::size_t idx = 0; idx < a.length; ++idx) {
    T *const p = a.data() + idx;
    p->~T();
  }
  a.length = 0;
}

template <typename T>
void
swap(Array<T> &f, Array<T> &s) noexcept {
  swap(f.buffer, s.buffer);
  swap(f.length, s.length);
  swap(f.capacity, s.capacity);
}

template <typename T, typename F>
void
for_each(Array<T> &a, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    f(a.buffer[i]);
  }
}

template <typename T, typename F>
void
for_each(const Array<T> &a, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const T &current = a.buffer[i];
    f(current);
  }
}

template <typename T, typename F>
bool
for_all(Array<T> &a, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < a.length && result; ++i) {
    result = f(a.buffer[i]);
  }
  return result;
}

template <typename T, typename F>
bool
for_all(const Array<T> &a, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; result && i < a.length; ++i) {
    const T &current = a.buffer[i];
    result = f(current);
  }
  return result;
}

template <typename T, typename Predicate>
T *
find(Array<T> &a, Predicate f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    if (f(a.buffer[i])) {
      return a.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, std::size_t c, typename Predicate>
T *
find(UinStaticArray<T, c> &a, Predicate f) noexcept {
  Array<T> ar(a.data(), a.length, c);
  return find(ar, f);
}

template <typename T, typename Predicate>
const T *
find(const Array<T> &a, Predicate f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const auto &current = a.buffer[i];
    if (f(current)) {
      return a.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, std::size_t c, typename Predicate>
T *
find(const UinStaticArray<T, c> &a, Predicate f) noexcept {
  const Array<T> ar(a.data(), a.length, c);
  return find(ar, f);
}

template <typename T, typename K, typename F>
K *
reduce(const Array<T> &a, K *k, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const T *current = a.buffer + i;
    k = f(k, current);
  }

  return k;
}

} // namespace sp

#endif
