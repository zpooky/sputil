#ifndef SP_UTIL_COLLECTION_ARRAY_H
#define SP_UTIL_COLLECTION_ARRAY_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <util/comparator.h>
#include <util/numeric.h>
#include <utility>
#include <util/assert.h>
#include <prng/util.h>

/*
 * TODO bin_search(arr, f -> -1|0|1)
 * TODO bin_remove
 */

namespace sp {
template <typename T>
struct Array {
  using value_type = T;

  T *buffer;
  std::size_t length;
  std::size_t capacity;

  Array() noexcept;

  template <std::size_t SIZE>
  explicit Array(T (&)[SIZE]) noexcept;

  Array(T *, std::size_t) noexcept;
  Array(T *, std::size_t, std::size_t) noexcept;

  T &operator[](std::size_t idx) noexcept {
    assertx(idx < length);
    return buffer[idx];
  }

  const T &operator[](std::size_t idx) const noexcept {
    assertx(idx < length);
    return buffer[idx];
  }
};

/*
 * Reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct StaticArray : public Array<T> {
  using value_type = T;

  T raw[cap];
  static constexpr std::size_t storage_capacity = cap;

  StaticArray() noexcept;
};

/*
 * Do not reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct UinStaticArray {
  using value_type = T;
  using storage_type =
      typename std::aligned_storage<sizeof(T) * cap, alignof(T)>::type;
  static constexpr std::size_t capacity = cap;

  storage_type buffer;
  std::size_t length;

  UinStaticArray() noexcept;
  UinStaticArray(const UinStaticArray &) = delete;
  UinStaticArray(const UinStaticArray &&) = delete;

  constexpr T *
  data() noexcept {
    return reinterpret_cast<T *>(&buffer);
  }

  constexpr const T *
  data() const noexcept {
    return reinterpret_cast<const T *>(&buffer);
  }

  T &operator[](std::size_t idx) noexcept {
    assertx(idx < length);
    return data()[idx];
  }

  const T &operator[](std::size_t idx) const noexcept {
    assertx(idx < length);
    return data()[idx];
  }
};

//=====================================
template <typename T>
std::size_t
length(const Array<T> &) noexcept;

template <typename T, std::size_t c>
std::size_t
length(const UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
std::size_t
capacity(const Array<T> &) noexcept;

template <typename T, std::size_t c>
std::size_t
capacity(const UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
std::size_t
remaining_write(const Array<T> &) noexcept;

template <typename T, std::size_t c>
std::size_t
remaining_write(const UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
bool
is_empty(const Array<T> &) noexcept;

template <typename T, std::size_t c>
bool
is_empty(const UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
bool
is_full(const Array<T> &) noexcept;

template <typename T, std::size_t c>
bool
is_full(const UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert(Array<T> &, V &&) noexcept;

template <typename T, std::size_t c, typename V,
          typename Comparator = sp::greater>
T *
bin_insert(UinStaticArray<T, c> &, V &&) noexcept;
//=====================================

template <typename T, typename K, typename Comparator = sp::greater>
T *
bin_search(Array<T> &, const K &) noexcept;

template <typename T, std::size_t c, typename K,
          typename Comparator = sp::greater>
T *
bin_search(UinStaticArray<T, c> &, const K &) noexcept;
//=====================================

// TODO const bin_search()
// TODO bin_remove()

// template <typename T>
// T *
// get(Array<T> &, std::size_t) noexcept;
//
// template <typename T, std::size_t c>
// T *
// get(UinStaticArray<T, c> &, std::size_t) noexcept;
//
// template <typename T>
// const T *
// get(const Array<T> &, std::size_t) noexcept;
//
// template <typename T, std::size_t c>
// const T *
// get(const UinStaticArray<T, c> &, std::size_t) noexcept;

//=====================================
/*
 * Insert a value at the back in the array. returns a pointer to the newly
 * inserted value or nullptr if the index is out of range.
 */
template <typename T, typename V>
T *
insert(Array<T> &, V &&) noexcept;

template <typename T, std::size_t c, typename V>
T *
insert(UinStaticArray<T, c> &, V &&) noexcept;
//=====================================

template <typename T, typename From>
bool
insert_all(Array<T> &, const From &) noexcept;

template <typename T, std::size_t c, typename From>
bool
insert_all(UinStaticArray<T, c> &, const From &) noexcept;

template <typename T, typename V>
bool
insert_all(Array<T> &, const V *, std::size_t) noexcept;

template <typename T, std::size_t c, typename V>
bool
insert_all(UinStaticArray<T, c> &, const V *, std::size_t) noexcept;
//=====================================

/*
 * Insert the value at the specified index. Values already present at and after
 * the index will be shifted upwards. returns a pointer to the newly inserted
 * value or nullptr if the index is out of range or the array is full.
 */
template <typename T, typename V>
T *
insert_at(Array<T> &, std::size_t, V &&) noexcept;

template <typename T, std::size_t c, typename V>
T *
insert_at(UinStaticArray<T, c> &, std::size_t, V &&) noexcept;
//=====================================

/*
 * Insert a value at a specific index, the index is required to be less than
 * Array.length. returns a pointer to the newly inserted value or nullptr if the
 * index is out of range. the old value will be dtor:ed
 */
template <typename T, typename V>
T *
set(Array<T> &, std::size_t, V &&) noexcept;

template <typename T, std::size_t c, typename V>
T *
set(UinStaticArray<T, c> &, std::size_t, V &&) noexcept;
//=====================================

/*
 * Insert a value at a specific index, the index is required to be less than
 * Array.length. the old existing value will be swap():ed with the new value.
 * returns a pointer to the newly inserted value or nullptr if the
 * index is out of range.
 */
template <typename T, typename V>
T *
exchange(Array<T> &, std::size_t, /*IN/OUT*/ V &) noexcept;

template <typename T, std::size_t c, typename V>
T *
exchange(UinStaticArray<T, c> &, std::size_t, /*IN/OUT*/ V &) noexcept;
//=====================================

/*
 * Takes the in pointer and calculates a index based on the head of the array.
 * if the pointer is out of bounds[Array.buffer-Array.length] Array.capacity is
 * returned.
 */
template <typename T>
std::size_t
index_of(const Array<T> &, const T *) noexcept;

template <typename T, std::size_t c>
std::size_t
index_of(const UinStaticArray<T, c> &, const T *) noexcept;
//=====================================

template <typename T>
bool
remove(Array<T> &, std::size_t idx) noexcept;

template <typename T, std::size_t c>
bool
remove(UinStaticArray<T, c> &, std::size_t idx) noexcept;

// TODO stable remove <- shifting everything after left
//=====================================

/*
 * Take element at $index and swap it with $out, then swap():ed element at
 * $index again with the last element in the Array and decrement Array.length.
 */
template <typename T>
bool
take(Array<T> &, std::size_t, /*OUT*/ T &) noexcept;

template <typename T, std::size_t c>
bool
take(UinStaticArray<T, c> &, std::size_t, /*OUT*/ T &) noexcept;
//=====================================
template <typename T>
const T *
last(const Array<T> &) noexcept;

template <typename T>
T *
last(Array<T> &) noexcept;

template <typename T, std::size_t c>
const T *
last(const UinStaticArray<T, c> &) noexcept;

template <typename T, std::size_t c>
T *
last(UinStaticArray<T, c> &) noexcept;

//=====================================
template <typename Random, typename T>
void
shuffle(Random &, Array<T> &) noexcept;

template <typename Random, typename T, std::size_t c>
void
shuffle(Random &, UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
void
clear(Array<T> &) noexcept;

template <typename T, std::size_t c>
void
clear(UinStaticArray<T, c> &) noexcept;
//=====================================

template <typename T>
void
drop_back(Array<T> &, std::size_t) noexcept;

template <typename T, std::size_t c>
void
drop_back(UinStaticArray<T, c> &, std::size_t) noexcept;
//=====================================

template <typename T>
void
swap(Array<T> &, Array<T> &) noexcept;
//=====================================

template <typename T, typename F>
void
for_each(Array<T> &, F) noexcept;

template <typename T, std::size_t c, typename F>
void
for_each(UinStaticArray<T, c> &, F) noexcept;

template <typename T, typename F>
void
for_each(const Array<T> &, F) noexcept;

template <typename T, std::size_t c, typename F>
void
for_each(const UinStaticArray<T, c> &, F) noexcept;
//=====================================

template <typename T, typename F>
bool
for_all(Array<T> &, F) noexcept;

template <typename T, std::size_t c, typename F>
bool
for_all(UinStaticArray<T, c> &, F) noexcept;

template <typename T, typename F>
bool
for_all(const Array<T> &, F) noexcept;

template <typename T, std::size_t c, typename F>
bool
for_all(const UinStaticArray<T, c> &, F) noexcept;

//=====================================
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

//=====================================
template <typename T, typename K, typename F>
K *
reduce(const Array<T> &, K *, F) noexcept;

template <typename T, std::size_t c, typename K, typename F>
K *
reduce(const UinStaticArray<T, c> &, K *, F) noexcept;

template <typename T, typename K, typename F>
K &
reduce(const Array<T> &, K &, F) noexcept;

template <typename T, std::size_t c, typename K, typename F>
K &
reduce(const UinStaticArray<T, c> &, K &, F) noexcept;
//=====================================

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

//=====================================
template <typename T>
std::size_t
length(const Array<T> &a) noexcept {
  return a.length;
}

template <typename T, std::size_t c>
std::size_t
length(const UinStaticArray<T, c> &a) noexcept {
  return a.length;
}

/* ===================================== */
template <typename T>
std::size_t
capacity(const Array<T> &a) noexcept {
  return a.capacity;
}

template <typename T, std::size_t c>
std::size_t
capacity(const UinStaticArray<T, c> &a) noexcept {
  return a.capacity;
}
//=====================================
template <typename T>
std::size_t
remaining_write(const Array<T> &a) noexcept {
  return capacity(a) - length(a);
}

template <typename T, std::size_t c>
std::size_t
remaining_write(const UinStaticArray<T, c> &a) noexcept {
  return capacity(a) - length(a);
}
//=====================================
template <typename T>
bool
is_empty(const Array<T> &a) noexcept {
  return length(a) == 0;
}

template <typename T, std::size_t c>
bool
is_empty(const UinStaticArray<T, c> &a) noexcept {
  return length(a) == 0;
}

//=====================================
template <typename T>
bool
is_full(const Array<T> &a) noexcept {
  return a.length == a.capacity;
}

template <typename T, std::size_t c>
bool
is_full(const UinStaticArray<T, c> &a) noexcept {
  return a.length == a.capacity;
}
//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert(Array<T> &, V &&) noexcept {
  assertx(false);
  // TODO
  return nullptr;
}

// TODO
template <typename T, std::size_t c, typename K, typename Comparator>
T *
bin_find_successor(UinStaticArray<T, c> &a, const K &needle) noexcept {
  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  auto seq_pred = [&needle](T *start, std::size_t len) -> T * {
    /**/
    for (std::size_t i = 0; i < len; ++i) {
      Comparator cmp;
      if (cmp(start[i], needle)) {
        return start + i;
      }
    }
    return nullptr;
  };

  if (a.length > 0) {
    std::size_t length = a.length;
    T *p = a.data();
  Lit:
    std::size_t mid = middle(length);
    // printf("%zu-[%zu]-%zu", p[0], p[mid], p[length - 1]);
    Comparator cmp;
    if /*needle < mid*/ (cmp(p[mid], needle)) {
      // printf("|mid[idx%zu] > needle[v%zu]|len:%zu\n", mid, needle, length);
      if (length == 1) {
        return p;
      }
      // length -= (mid);
      // length++;
      std::size_t before = length;
      length -= (mid);
      length++;
      if (before == length) {
        return seq_pred(p, length);
      }
      goto Lit;
    } /* needle > mid*/ else if (cmp(needle, p[mid])) {
      // printf("|n[v%zu] > mid[idx%zu]|len:%zu\n", needle, mid, length);
      p = p + mid;
      if (length > 1) {
        length -= mid;
        goto Lit;
      }
    } else {
      /* needle exact match */
      return p + mid;
    }

    // if (length > 1) {
    //   goto Lit;
    // }
  }

  return nullptr;
}

template <typename T, std::size_t c, typename V, typename Comparator>
T *
bin_insert(UinStaticArray<T, c> &a, V &&val) noexcept {
  if (a.length < a.capacity) {
    T *const first = bin_find_successor<T, c, V, Comparator>(/*>=*/a, val);
    // printf("------\n");
    T *it = insert(a, std::forward<V>(val));
    assertx(it);

    if (first) {
    Lit:
      using sp::swap;
      T *priv = it - 1;
      swap(*it, *priv);
      it = priv;
      if (priv != first) {
        goto Lit;
      }
    }
    return it;
  }

  return nullptr;
}

//=====================================
template <typename T, typename K, typename Comparator>
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
    /* think recursively:
     * p            mid         length
     * |____________|___________|
     * if needle < mid:
     * p     mid    length
     * |_____|______|
     * or needle > mid:
     *              p     mid   length
     *              |_____|_____|
     * ...
     */
    std::size_t mid = middle(length);
    Comparator cmp;
    if /*needle < mid*/ (cmp(p[mid], needle)) {

      /* p is unchanged */
    } /* needle > mid*/ else if (cmp(needle, p[mid])) {

      /* p is now $mid */
      p = p + mid;
    } /* !(needle <> mid)*/ else {

      /* match */
      return p + mid;
    }
    /* Since we have split in half the possible locations of $needle */
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
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return bin_search<T, K, Comparator>(c_a, needle);
}

//=====================================
template <typename T>
T *
get(Array<T> &a, std::size_t idx) noexcept {
  const auto &c_a = a;
  return (T *)get(c_a, idx);
}

template <typename T, std::size_t c>
T *
get(UinStaticArray<T, c> &a, std::size_t idx) noexcept {
  const auto &c_a = a;
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
  Array<T> c_a((T *)a.data(), a.length, c);
  return get(c_a, idx);
}
//=====================================
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
//=====================================

template <typename T, typename From>
bool
insert_all(Array<T> &a, const From &src) noexcept {
  std::size_t len = length(src);
  if (remaining_write(a) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      assertx(insert(a, src[i]));
    }

    return true;
  }

  return false;
}

template <typename T, std::size_t c, typename From>
bool
insert_all(UinStaticArray<T, c> &a, const From &src) noexcept {
  std::size_t len = length(src);
  if (remaining_write(a) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      assertx(insert(a, src[i]));
    }

    return true;
  }

  return false;
}

template <typename T, typename V>
bool
insert_all(Array<T> &a, const V *values, std::size_t length) noexcept {
  if (remaining_write(a) >= length) {
    for (std::size_t i = 0; i < length; ++i) {
      assertx(insert(a, values[i]));
    }

    return true;
  }

  return false;
}

template <typename T, std::size_t c, typename V>
bool
insert_all(UinStaticArray<T, c> &a, const V *values,
           std::size_t length) noexcept {
  if (remaining_write(a) >= length) {
    for (std::size_t i = 0; i < length; ++i) {
      assertx(insert(a, values[i]));
    }

    return true;
  }

  return false;
}

//=====================================
template <typename T, typename V>
T *
insert_at(Array<T> &, std::size_t, V &&) noexcept {
  // TODO
  assertx(false);
  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
insert_at(UinStaticArray<T, c> &a, std::size_t idx, V &&v) noexcept {
  if (idx <= a.length) {
    auto ins = insert(a, std::forward<V>(v));
    std::size_t ins_idx = index_of(a, ins);
    if (ins) {
      for (std::size_t i = ins_idx; i > idx; --i) {
        using sp::swap;
        swap(a.data()[i - 1], a.data()[i]);
      }

      return a.data() + idx;
    }
  }
  assertx(false);
  return nullptr;
}
//=====================================
template <typename T, typename V>
T *
set(Array<T> &a, std::size_t idx, V &&val) noexcept {
  if (idx <= length(a) && idx < capacity(a)) {
    T *const p = a.buffer + idx;
    p->~T();

    if (idx == a.length) {
      ++a.length;
    }

    return ::new (p) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
set(UinStaticArray<T, c> &a, std::size_t idx, V &&val) noexcept {
  if (idx <= length(a) && idx < capacity(a)) {
    T *const p = a.buffer + idx;

    if (idx == a.length) {
      ++a.length;
    }

    return ::new (p) T(std::forward<V>(val));
  }

  return nullptr;
}

//=====================================
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
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return exchange(c_a, idx, out);
}

//=====================================
template <typename T>
std::size_t
index_of(const Array<T> &a, const T *ptr) noexcept {
  if (ptr) {
    const T *start = a.buffer;
    auto s = reinterpret_cast<std::uintptr_t>(start);
    auto e = reinterpret_cast<std::uintptr_t>(ptr);
    if (e >= s) {
      auto index = e - s;
      // assertx(index % sizeof(*ptr) == 0);??
      index /= sizeof(*ptr);
      if (index < a.length) {
        return index;
      }
    }
  }
  return a.capacity;
}

template <typename T, std::size_t c>
std::size_t
index_of(const UinStaticArray<T, c> &a, const T *ptr) noexcept {
  const Array<T> c_a((T *)a.data(), a.length, a.capacity);
  return index_of(c_a, ptr);
}
//=====================================
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

//=====================================
template <typename T>
bool
take(Array<T> &a, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < a.length) {
    using sp::swap;
    swap(out, a.buffer[idx]);

    assertx(remove(a, idx));
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

    assertx(remove(a, idx));
    return true;
  }
  return false;
}
//=====================================

template <typename T>
const T *
last(const Array<T> &a) noexcept {
  if (is_empty(a))
    return nullptr;

  std::size_t idx = length(a) - 1;
  return get(a, idx);
}

template <typename T>
T *
last(Array<T> &a) noexcept {
  if (is_empty(a))
    return nullptr;

  std::size_t idx = length(a) - 1;
  return get(a, idx);
}

template <typename T, std::size_t c>
const T *
last(const UinStaticArray<T, c> &a) noexcept {
  if (is_empty(a))
    return nullptr;

  std::size_t idx = length(a) - 1;
  return get(a, idx);
}

template <typename T, std::size_t c>
T *
last(UinStaticArray<T, c> &a) noexcept {
  if (is_empty(a))
    return nullptr;

  std::size_t idx = length(a) - 1;
  return get(a, idx);
}

//=====================================
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
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  Array<T> c_a(a.data(), a.length, cap);
  return shuffle(r, c_a);
}

//=====================================
template <typename T>
void
clear(Array<T> &a) noexcept {
  drop_back(a, length(a));
}

template <typename T, std::size_t c>
void
clear(UinStaticArray<T, c> &a) noexcept {
  drop_back(a, length(a));
}

//=====================================
template <typename T>
void
drop_back(Array<T> &a, std::size_t len) noexcept {
  assertx(len <= length(a));

  length = std::min(length(a), len);
  for (std::size_t idx = len; idx-- > 0;) {
    T *const p = a.buffer + idx;
    p->~T();
  }
  a.length -= len;
}

template <typename T, std::size_t c>
void
drop_back(UinStaticArray<T, c> &a, std::size_t len) noexcept {
  assertx(len <= length(a));

  len = std::min(length(a), len);
  for (std::size_t idx = len; idx-- > 0;) {
    T *const p = a.data() + idx;
    p->~T();
  }
  a.length -= len;
}

//=====================================
template <typename T>
void
swap(Array<T> &f, Array<T> &s) noexcept {
  // TODO we should not support the static variations of Array!
  swap(f.buffer, s.buffer);
  swap(f.length, s.length);
  swap(f.capacity, s.capacity);
}

//=====================================
template <typename T, typename F>
void
for_each(Array<T> &a, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    f(a.buffer[i]);
  }
}

template <typename T, std::size_t c, typename F>
void
for_each(UinStaticArray<T, c> &a, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    f(a.data()[i]);
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

template <typename T, std::size_t c, typename F>
void
for_each(const UinStaticArray<T, c> &a, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const T &current = a.data()[i];
    f(current);
  }
}

//=====================================
template <typename T, typename F>
bool
for_all(Array<T> &a, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < a.length && result; ++i) {
    result = f(a.buffer[i]);
  }
  return result;
}
template <typename T, std::size_t c, typename F>
bool
for_all(UinStaticArray<T, c> &a, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < a.length && result; ++i) {
    result = f(a.data()[i]);
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

template <typename T, std::size_t c, typename F>
bool
for_all(const UinStaticArray<T, c> &a, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; result && i < a.length; ++i) {
    const T &current = a.data()[i];
    result = f(current);
  }
  return result;
}

//=====================================
template <typename T, typename Predicate>
T *
find(Array<T> &a, Predicate f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    T &current = a.buffer[i];
    if (f(current)) {
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

//=====================================
template <typename T, typename K, typename F>
K *
reduce(const Array<T> &a, K *k, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const T *current = a.buffer + i;
    k = f(k, current);
  }

  return k;
}

template <typename T, std::size_t c, typename K, typename F>
K *
reduce(const UinStaticArray<T, c> &a, K *k, F f) noexcept {
  const Array<T> ar((T *)a.data(), a.length, c);
  return reduce(ar, k, f);
}

template <typename T, typename K, typename F>
K &
reduce(const Array<T> &a, K &k, F f) noexcept {
  for (std::size_t i = 0; i < a.length; ++i) {
    const T &current = a.buffer[i];
    k = f(k, current);
  }

  return k;
}

template <typename T, std::size_t c, typename K, typename F>
K &
reduce(const UinStaticArray<T, c> &a, K &k, F f) noexcept {
  const Array<T> ar((T *)a.data(), a.length, c);
  return reduce(ar, k, f);
}

} // namespace sp

#endif
