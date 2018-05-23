#ifndef SP_UTIL_COLLECTION_ARRAY_H
#define SP_UTIL_COLLECTION_ARRAY_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <prng/util.h>
#include <util/assert.h>
#include <util/comparator.h>
#include <util/numeric.h>
#include <utility>

/*
 * TODO bin_search(arr, f -> -1|0|1)
 */

namespace sp {
//=====================================
// Note: pointer supplied to the ctor is required to point to ctor:ed objects
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

  T &operator[](std::size_t idx) noexcept;
  const T &operator[](std::size_t idx) const noexcept;

  T *
  data() noexcept;

  const T *
  data() const noexcept;
};

//=====================================
/*
 * Reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct StaticArray : public Array<T> {
  using value_type = T;

  T raw[cap];
  static constexpr std::size_t storage_capacity = cap;

  StaticArray() noexcept;
  StaticArray(std::initializer_list<T>) noexcept;
};

//=====================================
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
  UinStaticArray(UinStaticArray &&) noexcept;

  ~UinStaticArray() noexcept;

  T *
  data() noexcept;

  const T *
  data() const noexcept;

  T &operator[](std::size_t idx) noexcept;
  const T &operator[](std::size_t idx) const noexcept;
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

template <typename T, std::size_t c, typename V, typename C = sp::greater>
T *
bin_insert(UinStaticArray<T, c> &, V &&) noexcept;

//=====================================
template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert_unique(Array<T> &, V &&) noexcept;

template <typename T, std::size_t c, typename V, typename C = sp::greater>
T *
bin_insert_unique(UinStaticArray<T, c> &, V &&) noexcept;

//=====================================
template <typename T, typename K, typename Comparator = sp::greater>
const T *
bin_search(const Array<T> &, const K &) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
T *
bin_search(Array<T> &, const K &) noexcept;

template <typename T, std::size_t c, typename K, typename C = sp::greater>
const T *
bin_search(const UinStaticArray<T, c> &, const K &) noexcept;

template <typename T, std::size_t c, typename K, typename C = sp::greater>
T *
bin_search(UinStaticArray<T, c> &, const K &) noexcept;

//=====================================
template <typename T, std::size_t c, typename K, typename C = sp::greater>
bool
bin_remove(UinStaticArray<T, c> &, const K &) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
bool
bin_remove(Array<T> &, const K &) noexcept;

//=====================================
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

//=====================================
template <typename T, typename Predicate>
void
remove_if(Array<T> &, Predicate) noexcept;

template <typename T, std::size_t c, typename Predicate>
void
remove_if(UinStaticArray<T, c> &, Predicate) noexcept;

//=====================================
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
//====Implementation===================
//=====================================
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

template <typename T>
T &Array<T>::operator[](std::size_t idx) noexcept {
  assertxs(idx < length, idx, length);
  return buffer[idx];
}

template <typename T>
const T &Array<T>::operator[](std::size_t idx) const noexcept {
  assertxs(idx < length, idx, length);
  return buffer[idx];
}

template <typename T>
T *
Array<T>::data() noexcept {
  return buffer;
}

template <typename T>
const T *
Array<T>::data() const noexcept {
  return buffer;
}

//=====================================
template <typename T, std::size_t c>
StaticArray<T, c>::StaticArray() noexcept
    : Array<T>(raw)
    , raw{} {
}

template <typename T, std::size_t c>
StaticArray<T, c>::StaticArray(std::initializer_list<T> l) noexcept
    : StaticArray() {
  assertx(l.size() <= capacity(*this));
  bool result = insert_all(*this, l.begin(), l.size());
  assertx(result);
}

//=====================================
template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray() noexcept
    : buffer{}
    , length{0} {
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray(UinStaticArray &&o) noexcept
    : UinStaticArray() {
  for (std::size_t i = 0; i < sp::length(o); ++i) {
    auto res = insert(*this, std::move(o[i]));
    assertx(res);
    o[i].~T();
  }
  o.length = 0;
}

template <typename T, std::size_t c>
T *
UinStaticArray<T, c>::data() noexcept {
  return reinterpret_cast<T *>(&buffer);
}

template <typename T, std::size_t c>
const T *
UinStaticArray<T, c>::data() const noexcept {
  return reinterpret_cast<const T *>(&buffer);
}

template <typename T, std::size_t c>
T &UinStaticArray<T, c>::operator[](std::size_t idx) noexcept {
  assertxs(idx < length, idx, length);
  return data()[idx];
}

template <typename T, std::size_t c>
const T &UinStaticArray<T, c>::operator[](std::size_t idx) const noexcept {
  assertxs(idx < length, idx, length);
  return data()[idx];
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::~UinStaticArray() noexcept {
  for (std::size_t i = 0; i < length; ++i) {
    this->operator[](i).~T();
  }
  length = 0;
}

//=====================================
template <typename T>
std::size_t
length(const Array<T> &self) noexcept {
  return self.length;
}

template <typename T, std::size_t c>
std::size_t
length(const UinStaticArray<T, c> &self) noexcept {
  return self.length;
}

/* ===================================== */
template <typename T>
std::size_t
capacity(const Array<T> &self) noexcept {
  return self.capacity;
}

template <typename T, std::size_t c>
std::size_t
capacity(const UinStaticArray<T, c> &self) noexcept {
  return self.capacity;
}
//=====================================
template <typename T>
std::size_t
remaining_write(const Array<T> &self) noexcept {
  return capacity(self) - length(self);
}

template <typename T, std::size_t c>
std::size_t
remaining_write(const UinStaticArray<T, c> &self) noexcept {
  return capacity(self) - length(self);
}
//=====================================
template <typename T>
bool
is_empty(const Array<T> &self) noexcept {
  return length(self) == 0;
}

template <typename T, std::size_t c>
bool
is_empty(const UinStaticArray<T, c> &self) noexcept {
  return length(self) == 0;
}

//=====================================
template <typename T>
bool
is_full(const Array<T> &self) noexcept {
  return self.length == self.capacity;
}

template <typename T, std::size_t c>
bool
is_full(const UinStaticArray<T, c> &self) noexcept {
  return self.length == self.capacity;
}
//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert(Array<T> &, V &&) noexcept {
  assertx(false);
  // TODO
  return nullptr;
}

template <typename T, std::size_t c, typename K, typename Comparator>
static const T *
bin_find_gte(const UinStaticArray<T, c> &self, const K &needle) noexcept {
  if (self.length == 0) {
    return nullptr;
  }

  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  std::size_t length = self.length;
  const T *p = self.data();
  const T *const end = p + length;
Lit : {
  const std::size_t mid = middle(length);
  // printf("%zu-[%zu]-%zu", p[0], p[mid], p[length - 1]);
  Comparator cmp;
  if /*needle < mid*/ (cmp(p[mid], /*>*/ needle)) {
    // printf("|mid[idx%zu] > needle[v%zu]|len:%zu\n", mid, needle, length);
    if (length == 0) {
      return p;
    }

    length = mid;

    goto Lit;
  } /* needle > mid*/ else if (cmp(needle, /*>*/ p[mid])) {
    // printf("|n[v%zu] > mid[idx%zu]|len:%zu\n", needle, mid, length);
    if (length == 1) {
      if (p + length != end) {
        return p + length;
      } else {
        return nullptr;
      }
    }

    p = p + mid;
    length = length - mid;

    goto Lit;
  } else {
    /* needle is exact match */
    return p + mid;
  }
}

  return nullptr;
}

template <typename T, std::size_t c, typename K, typename Comparator>
static T *
bin_find_gte(UinStaticArray<T, c> &self, const K &needle) noexcept {
  const auto &c_self = self;
  return (T *)bin_find_gte<T, c, K, Comparator>(c_self, needle);
}

template <typename T, std::size_t c, typename V, typename Comparator>
T *
bin_insert(UinStaticArray<T, c> &self, V &&val) noexcept {
  if (!is_full(self)) {
    T *const first = bin_find_gte<T, c, V, Comparator>(/*>=*/self, val);
    // printf("------\n");
    T *it = insert(self, std::forward<V>(val));
    assertx(it);

    if (first) {
    // shift down
    Lit:
      T *priv = it - 1;
      using std::swap;
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
template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(Array<T> &, V &&) noexcept {
  assertx(false);
  return nullptr;
}

template <typename T, std::size_t c, typename V, typename Comparator>
T *
bin_insert_unique(UinStaticArray<T, c> &self, V &&val) noexcept {
  if (self.length < self.capacity) {
    T *const first = bin_find_gte<T, c, V, Comparator>(/*>=*/self, val);
    if (first) {
      Comparator cmp;
      if (!cmp(val, *first) && !cmp(*first, val)) {
        // equal
        return nullptr;
      }
    }
    // copy from bin_insert {
    T *it = insert(self, std::forward<V>(val));
    assertx(it);

    if (first) {
    // shift down
    Lit:
      T *priv = it - 1;
      using std::swap;
      swap(*it, *priv);
      it = priv;
      if (priv != first) {
        goto Lit;
      }
    }
    return it;
  }
  // }
  return nullptr;
}

//=====================================
template <typename T, typename K, typename Comparator>
const T *
bin_search(const Array<T> &self, const K &needle) noexcept {
  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  if (self.length > 0) {

    std::size_t length = self.length;
    const T *p = self.buffer;
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

template <typename T, typename K, typename Comparator>
T *
bin_search(Array<T> &self, const K &needle) noexcept {
  return (T *)bin_search<T, K, Comparator>((const Array<T> &)self, needle);
}

template <typename T, std::size_t c, typename K, typename Comparator>
const T *
bin_search(const UinStaticArray<T, c> &self, const K &needle) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  const Array<T> c_self((T *)self.data(), self.length, cap);
  return bin_search<T, K, Comparator>(c_self, needle);
}

template <typename T, std::size_t c, typename K, typename Comparator>
T *
bin_search(UinStaticArray<T, c> &self, const K &needle) noexcept {
  const UinStaticArray<T, c> &c_self = self;
  return (T *)bin_search<T, c, K, Comparator>(c_self, needle);
}

//=====================================
template <typename T, std::size_t c, typename K, typename Comparator>
bool
bin_remove(UinStaticArray<T, c> &self, const K &needle) noexcept {
  T *const res = bin_search(self, needle);
  if (res) {
    std::size_t rem_idx = index_of(self, res);
    for (; (rem_idx + 1) < self.length;) {
      using std::swap;
      T &first = self.data()[rem_idx];
      T &second = self.data()[++rem_idx];
      swap(first, second);
    }
    assertxs(rem_idx == (self.length - 1), rem_idx, self.length);
    self.data()[--self.length].~T();

    return true;
  }

  return false;
}

template <typename T, typename K, typename Comparator>
bool
bin_remove(Array<T> &, const K &) noexcept {
  assertx(false);
  return false;
}

//=====================================
template <typename T>
T *
get(Array<T> &self, std::size_t idx) noexcept {
  const auto &c_self = self;
  return (T *)get(c_self, idx);
}

template <typename T, std::size_t c>
T *
get(UinStaticArray<T, c> &self, std::size_t idx) noexcept {
  const auto &c_self = self;
  return get(c_self, idx);
}

template <typename T>
const T *
get(const Array<T> &self, std::size_t idx) noexcept {
  if (idx < self.length) {
    return self.buffer + idx;
  }
  return nullptr;
}

template <typename T, std::size_t c>
T *
get(const UinStaticArray<T, c> &self, std::size_t idx) noexcept {
  Array<T> c_self((T *)self.data(), self.length, c);
  return get(c_self, idx);
}
//=====================================
template <typename T, typename V>
T *
insert(Array<T> &self, V &&val) noexcept {
  if (self.length < self.capacity) {
    std::size_t idx = self.length++;
    T *const raw = self.buffer + idx;

    // dtor of default init T
    raw->~T();

    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
insert(UinStaticArray<T, c> &self, V &&val) noexcept {
  if (self.length < c) {
    std::size_t idx = self.length++;
    T *const raw = self.data() + idx;

    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}
//=====================================

template <typename T, typename From>
bool
insert_all(Array<T> &self, const From &src) noexcept {
  std::size_t len = length(src);
  if (remaining_write(self) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      assertx(insert(self, src[i]));
    }

    return true;
  }

  return false;
}

template <typename T, std::size_t c, typename From>
bool
insert_all(UinStaticArray<T, c> &self, const From &src) noexcept {
  std::size_t len = length(src);
  if (remaining_write(self) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      assertx(insert(self, src[i]));
    }

    return true;
  }

  return false;
}

template <typename T, typename V>
bool
insert_all(Array<T> &self, const V *values, std::size_t length) noexcept {
  if (remaining_write(self) >= length) {
    for (std::size_t i = 0; i < length; ++i) {
      assertx(insert(self, values[i]));
    }

    return true;
  }

  return false;
}

template <typename T, std::size_t c, typename V>
bool
insert_all(UinStaticArray<T, c> &self, const V *values,
           std::size_t length) noexcept {
  if (remaining_write(self) >= length) {
    for (std::size_t i = 0; i < length; ++i) {
      assertx(insert(self, values[i]));
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
insert_at(UinStaticArray<T, c> &self, std::size_t idx, V &&v) noexcept {
  if (is_full(self)) {
    return nullptr;
  }

  if (idx <= self.length) {
    auto ins = insert(self, std::forward<V>(v));
    assertx(ins);
    std::size_t ins_idx = index_of(self, ins);
    if (ins) {
      for (std::size_t i = ins_idx; i > idx; --i) {
        using std::swap;
        auto &first = self.data()[i - 1];
        auto &second = self.data()[i];
        swap(first, second);
      }

      return self.data() + idx;
    }
  }

  assertx(false);
  return nullptr;
}
//=====================================
template <typename T, typename V>
T *
set(Array<T> &self, std::size_t idx, V &&val) noexcept {
  if (idx <= length(self) && idx < capacity(self)) {
    T *const p = self.buffer + idx;
    p->~T();

    if (idx == self.length) {
      ++self.length;
    }

    return ::new (p) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
set(UinStaticArray<T, c> &self, std::size_t idx, V &&val) noexcept {
  if (idx <= length(self) && idx < capacity(self)) {
    T *const p = self.buffer + idx;

    if (idx == self.length) {
      ++self.length;
    }

    return ::new (p) T(std::forward<V>(val));
  }

  return nullptr;
}

//=====================================
template <typename T, typename V>
T *
exchange(Array<T> &self, std::size_t idx, /*IN/OUT*/ V &out) noexcept {
  if (idx < self.length) {
    using std::swap;
    swap(self.buffer[idx], out);
    return self.buffer + idx;
  }
  return nullptr;
}

template <typename T, std::size_t c, typename V>
T *
exchange(UinStaticArray<T, c> &self, std::size_t idx,
         /*IN/OUT*/ V &out) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  Array<T> c_self(self.data(), self.length, cap);
  return exchange(c_self, idx, out);
}

//=====================================
template <typename T>
std::size_t
index_of(const Array<T> &self, const T *ptr) noexcept {
  if (ptr) {
    const T *start = self.buffer;
    auto s = reinterpret_cast<std::uintptr_t>(start);
    auto e = reinterpret_cast<std::uintptr_t>(ptr);
    if (e >= s) {
      auto index = e - s;
      // assertx(index % sizeof(*ptr) == 0);??
      index /= sizeof(*ptr);
      if (index < self.length) {
        return index;
      }
    }
  }
  return self.capacity;
}

template <typename T, std::size_t c>
std::size_t
index_of(const UinStaticArray<T, c> &self, const T *ptr) noexcept {
  const Array<T> c_self((T *)self.data(), self.length, self.capacity);
  return index_of(c_self, ptr);
}

//=====================================
template <typename T>
bool
remove(Array<T> &self, std::size_t idx) noexcept {
  assertxs(idx < length(self), idx, length(self));

  if (idx < self.length) {
    std::size_t last = --self.length;
    if (idx != last) {
      using std::swap;
      swap(self.buffer[idx], self.buffer[last]);
    }

    T *const raw = self.buffer + last;
    raw->~T();
    new (raw) T;
    return true;
  }

  return false;
}

template <typename T, std::size_t c>
bool
remove(UinStaticArray<T, c> &self, std::size_t idx) noexcept {
  assertxs(idx < length(self), idx, length(self));
  /*
   * Differing only in that we do not defalt construct the empty element
   */
  if (idx < length(self)) {
    std::size_t last = --self.length;
    if (idx != last) {
      using std::swap;
      swap(self.data()[idx], self.data()[last]);
    }

    T *const raw = self.data() + last;
    raw->~T();

    return true;
  }

  return false;
}

//=====================================
template <typename T, typename Predicate>
void
remove_if(Array<T> &, Predicate) noexcept {
  assertx(false);
}

template <typename T, std::size_t c, typename Predicate>
void
remove_if(UinStaticArray<T, c> &self, Predicate p) noexcept {
  T *it = self.data();
  while (it != (self.data() + length(self))) {
    const bool del = p(*it);
    if (del) {
      std::size_t index = index_of(self, it);
      assertx(index != capacity(self));

      bool res = remove(self, index);
      assertxs(res, res);
    } else {
      ++it;
    }
  }
}

//=====================================
template <typename T>
bool
take(Array<T> &self, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < self.length) {
    using std::swap;
    swap(out, self.buffer[idx]);

    bool res = remove(self, idx);
    assertx(res);

    return true;
  }

  return false;
}

template <typename T, std::size_t c>
bool
take(UinStaticArray<T, c> &self, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < self.length) {
    using std::swap;
    swap(out, self.data()[idx]);

    bool res = remove(self, idx);
    assertx(res);

    return true;
  }

  return false;
}

//=====================================
template <typename T>
const T *
last(const Array<T> &self) noexcept {
  if (is_empty(self))
    return nullptr;

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

template <typename T>
T *
last(Array<T> &self) noexcept {
  if (is_empty(self))
    return nullptr;

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

template <typename T, std::size_t c>
const T *
last(const UinStaticArray<T, c> &self) noexcept {
  if (is_empty(self))
    return nullptr;

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

template <typename T, std::size_t c>
T *
last(UinStaticArray<T, c> &self) noexcept {
  if (is_empty(self))
    return nullptr;

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

//=====================================
template <typename Random, typename T>
void
shuffle(Random &r, Array<T> &self) noexcept {
  for (std::size_t idx = 0; idx < self.length; ++idx) {
    auto replace =
        uniform_dist(r, std::uint32_t(0), std::uint32_t(self.length));
    if (idx != replace) {
      using std::swap;
      swap(self.buffer[idx], self.buffer[replace]);
    }
  }
}

template <typename Random, typename T, std::size_t c>
void
shuffle(Random &r, UinStaticArray<T, c> &self) noexcept {
  constexpr std::size_t cap = UinStaticArray<T, c>::capacity;
  Array<T> c_self(self.data(), self.length, cap);
  return shuffle(r, c_self);
}

//=====================================
template <typename T>
void
clear(Array<T> &self) noexcept {
  drop_back(self, length(self));
}

template <typename T, std::size_t c>
void
clear(UinStaticArray<T, c> &self) noexcept {
  drop_back(self, length(self));
}

//=====================================
template <typename T>
void
drop_back(Array<T> &self, std::size_t len) noexcept {
  assertx(len <= length(self));

  length = std::min(length(self), len);
  for (std::size_t idx = len; idx-- > 0;) {
    T *const p = self.buffer + idx;
    p->~T();
  }
  self.length -= len;
}

template <typename T, std::size_t c>
void
drop_back(UinStaticArray<T, c> &self, std::size_t len) noexcept {
  assertx(len <= length(self));

  len = std::min(length(self), len);
  for (std::size_t idx = len; idx-- > 0;) {
    T *const p = self.data() + idx;
    p->~T();
  }
  self.length -= len;
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
for_each(Array<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    f(self.buffer[i]);
  }
}

template <typename T, std::size_t c, typename F>
void
for_each(UinStaticArray<T, c> &self, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    f(self.data()[i]);
  }
}

template <typename T, typename F>
void
for_each(const Array<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    const T &current = self.buffer[i];
    f(current);
  }
}

template <typename T, std::size_t c, typename F>
void
for_each(const UinStaticArray<T, c> &self, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    const T &current = self.data()[i];
    f(current);
  }
}

//=====================================
template <typename T, typename F>
bool
for_all(Array<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < self.length && result; ++i) {
    result = f(self.buffer[i]);
  }
  return result;
}
template <typename T, std::size_t c, typename F>
bool
for_all(UinStaticArray<T, c> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < self.length && result; ++i) {
    result = f(self.data()[i]);
  }
  return result;
}

template <typename T, typename F>
bool
for_all(const Array<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; result && i < self.length; ++i) {
    const T &current = self.buffer[i];
    result = f(current);
  }
  return result;
}

template <typename T, std::size_t c, typename F>
bool
for_all(const UinStaticArray<T, c> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; result && i < self.length; ++i) {
    const T &current = self.data()[i];
    result = f(current);
  }
  return result;
}

//=====================================
template <typename T, typename Predicate>
T *
find(Array<T> &self, Predicate f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    T &current = self.buffer[i];
    if (f(current)) {
      return self.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, std::size_t c, typename Predicate>
T *
find(UinStaticArray<T, c> &self, Predicate f) noexcept {
  Array<T> ar(self.data(), self.length, c);
  return find(ar, f);
}

template <typename T, typename Predicate>
const T *
find(const Array<T> &self, Predicate f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    const auto &current = self.buffer[i];
    if (f(current)) {
      return self.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, std::size_t c, typename Predicate>
T *
find(const UinStaticArray<T, c> &self, Predicate f) noexcept {
  const Array<T> ar(self.data(), self.length, c);
  return find(ar, f);
}

//=====================================
template <typename T, typename K, typename F>
K *
reduce(const Array<T> &self, K *k, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    const T *current = self.buffer + i;
    k = f(k, current);
  }

  return k;
}

template <typename T, std::size_t c, typename K, typename F>
K *
reduce(const UinStaticArray<T, c> &self, K *k, F f) noexcept {
  const Array<T> ar((T *)self.data(), self.length, c);
  return reduce(ar, k, f);
}

template <typename T, typename K, typename F>
K &
reduce(const Array<T> &self, K &k, F f) noexcept {
  for (std::size_t i = 0; i < self.length; ++i) {
    const T &current = self.buffer[i];
    k = f(k, current);
  }

  return k;
}

template <typename T, std::size_t c, typename K, typename F>
K &
reduce(const UinStaticArray<T, c> &self, K &k, F f) noexcept {
  const Array<T> ar((T *)self.data(), self.length, c);
  return reduce(ar, k, f);
}

} // namespace sp

#endif
