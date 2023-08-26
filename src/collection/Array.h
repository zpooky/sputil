#ifndef SP_UTIL_COLLECTION_ARRAY_H
#define SP_UTIL_COLLECTION_ARRAY_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <prng/util.h>
#include <util/array.h>
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

  virtual ~Array() noexcept;

  template <std::size_t SIZE>
  explicit Array(T (&)[SIZE]) noexcept;

  Array(T *, std::size_t) noexcept;
  Array(T *, std::size_t, std::size_t) noexcept;

  T &
  operator[](std::size_t idx) noexcept;
  const T &
  operator[](std::size_t idx) const noexcept;

  T *
  data() noexcept;
  const T *
  data() const noexcept;

  T *
  begin() noexcept;
  const T *
  begin() const noexcept;

  T *
  end() noexcept;
  const T *
  end() const noexcept;

  bool
  operator==(const Array &o) const noexcept;

  template <typename T2, std::size_t SIZE>
  bool
  operator==(const T2 (&)[SIZE]) const noexcept;
};

//=====================================
/*
 * Reuquire a T default ctor
 */
template <typename T>
struct DynamicArray : public Array<T> {

  DynamicArray(std::size_t) noexcept;
  DynamicArray(std::initializer_list<T>) noexcept;

  ~DynamicArray() noexcept;
};

//=====================================
/*
 * Reuquire a T default ctor
 */
template <typename T, std::size_t cap>
struct StaticArray : public Array<T> {
  using value_type = T;
  static constexpr std::size_t storage_capacity = cap;

  T raw[cap];

  StaticArray() noexcept;
  StaticArray(std::initializer_list<T>) noexcept;
};

//=====================================
/*
 * Does not reuquire a T default ctor
 */
template <typename T>
struct UinArray {
protected:
  using storage_type =
      typename std::aligned_storage<sizeof(T), alignof(T)>::type;

public:
  using value_type = T;

  storage_type *buffer;
  std::size_t capacity;
  std::size_t length;

  UinArray(storage_type *buffer, std::size_t c) noexcept;
  UinArray() noexcept;

  UinArray(const UinArray &) = delete;
  UinArray(const UinArray &&) = delete;

  virtual ~UinArray() noexcept {
  }

  T *
  data() noexcept;
  const T *
  data() const noexcept;

  T *
  begin() noexcept {
    return data();
  }
  const T *
  begin() const noexcept {
    return data();
  }

  T *
  end() noexcept;
  const T *
  end() const noexcept;

  T &
  operator[](std::size_t idx) noexcept;
  const T &
  operator[](std::size_t idx) const noexcept;

  bool
  operator==(const UinArray &o) const noexcept;

  template <typename T2, std::size_t SIZE>
  bool
  operator==(const T2 (&)[SIZE]) const noexcept;
};

//=====================================
template <typename T, std::size_t cap>
struct UinStaticArray : public UinArray<T> {

  static_assert(cap > 0, "");

  static constexpr std::size_t capacity = cap;

  // using storage_type =
  //    typename std::aligned_storage<sizeof(T) * cap, alignof(T)>::type;
  typename UinArray<T>::storage_type raw[cap];

  UinStaticArray() noexcept;

  UinStaticArray(const UinStaticArray<T, cap> &) = delete;

  UinStaticArray(UinStaticArray<T, cap> &&) noexcept;
  UinStaticArray(std::initializer_list<T>) noexcept;

  virtual ~UinStaticArray() noexcept;
};

//=====================================
template <typename T>
struct UinDynamicArray : public UinArray<T> {
  UinDynamicArray(std::size_t cap) noexcept;

  UinDynamicArray(const UinDynamicArray<T> &) = delete;

  UinDynamicArray(UinDynamicArray<T> &&) noexcept;
  UinDynamicArray(std::initializer_list<T>) noexcept;

  virtual ~UinDynamicArray() noexcept;
};

//=====================================
template <typename T>
std::size_t
length(const Array<T> &) noexcept;

template <typename T>
std::size_t
length(const UinArray<T> &) noexcept;

//=====================================
template <typename T>
std::size_t
capacity(const Array<T> &) noexcept;

template <typename T>
std::size_t
capacity(const UinArray<T> &) noexcept;

//=====================================
template <typename T>
std::size_t
remaining_write(const Array<T> &) noexcept;

template <typename T>
std::size_t
remaining_write(const UinArray<T> &) noexcept;

//=====================================
template <typename T>
bool
is_empty(const Array<T> &) noexcept;

template <typename T>
bool
is_empty(const UinArray<T> &) noexcept;
//=====================================
template <typename T>
bool
is_full(const Array<T> &) noexcept;

template <typename T>
bool
is_full(const UinArray<T> &) noexcept;

//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert(Array<T> &, V &&, Comparator &) noexcept;

template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert(Array<T> &, V &&) noexcept;

template <typename T, typename V, typename C>
T *
bin_insert(UinArray<T> &, V &&, C &) noexcept;

template <typename T, typename V, typename C = sp::greater>
T *
bin_insert(UinArray<T> &, V &&) noexcept;

//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(Array<T> &, V &&, Comparator &cmp) noexcept;

template <typename T, typename V, typename Comparator = sp::greater>
T *
bin_insert_unique(Array<T> &, V &&) noexcept;

template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(UinArray<T> &, V &&, Comparator &) noexcept;

template <typename T, typename V, typename C = sp::greater>
T *
bin_insert_unique(UinArray<T> &, V &&) noexcept;

//=====================================
template <typename T, typename K, typename Comparator>
const T *
bin_search(const Array<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
const T *
bin_search(const Array<T> &, const K &) noexcept;

template <typename T, typename K, typename Comparator>
T *
bin_search(Array<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
T *
bin_search(Array<T> &, const K &) noexcept;

template <typename T, typename K, typename Comparator>
const T *
bin_search(const UinArray<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename C = sp::greater>
const T *
bin_search(const UinArray<T> &, const K &) noexcept;

template <typename T, typename K, typename Comparator>
T *
bin_search(UinArray<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename C = sp::greater>
T *
bin_search(UinArray<T> &, const K &) noexcept;

//=====================================
template <typename T, typename K, typename Comparator>
bool
bin_remove(UinArray<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename C = sp::greater>
bool
bin_remove(UinArray<T> &, const K &) noexcept;

template <typename T, typename K, typename Comparator>
bool
bin_remove(Array<T> &, const K &, Comparator &) noexcept;

template <typename T, typename K, typename Comparator = sp::greater>
bool
bin_remove(Array<T> &, const K &) noexcept;

//=====================================
// template <typename T>
// T *
// get(Array<T> &, std::size_t) noexcept;
//
// template <typename T>
// T *
// get(UinArray<T> &, std::size_t) noexcept;
//
// template <typename T>
// const T *
// get(const Array<T> &, std::size_t) noexcept;
//
// template <typename T>
// const T *
// get(const UinArray<T> &, std::size_t) noexcept;

//=====================================
/*
 * Insert a value at the back in the array. returns a pointer to the newly
 * inserted value or nullptr if the index is out of range.
 */
template <typename T, typename V>
T *
insert(Array<T> &, V &&) noexcept;

template <typename T, typename V>
T *
insert(UinArray<T> &, V &&) noexcept;

//=====================================
template <typename T, typename From>
bool
insert_all(Array<T> &, const From &) noexcept;

template <typename T, typename From>
bool
insert_all(UinArray<T> &, const From &) noexcept;

template <typename T, typename V>
bool
insert_all(Array<T> &, const V *, std::size_t) noexcept;

template <typename T, typename V>
bool
insert_all(UinArray<T> &, const V *, std::size_t) noexcept;

//=====================================
template <typename T>
bool
move_all(UinArray<T> &, Array<T> &) noexcept;

template <typename T>
bool
move_all(UinArray<T> &, UinArray<T> &) noexcept;

//=====================================
/* The same as insert()
 */
template <typename T, typename V>
T *
push(Array<T> &, V &&) noexcept;

template <typename T, typename V>
T *
push(UinArray<T> &, V &&) noexcept;

//=====================================
template <typename T>
bool
pop(Array<T> &) noexcept;

template <typename T>
bool
pop(UinArray<T> &) noexcept;

//=====================================
/*
 * Insert the value at the specified index. Values already present at and after
 * the index will be shifted upwards. returns a pointer to the newly inserted
 * value or nullptr if the index is out of range or the array is full.
 */
template <typename T, typename V>
T *
insert_at(Array<T> &, std::size_t, V &&) noexcept;

template <typename T, typename V>
T *
insert_at(UinArray<T> &, std::size_t, V &&) noexcept;

//=====================================
/*
 * Insert a value at a specific index, the index is required to be less than
 * Array.length. returns a pointer to the newly inserted value or nullptr if the
 * index is out of range. the old value will be dtor:ed
 */
template <typename T, typename V>
T *
set(Array<T> &, std::size_t, V &&) noexcept;

template <typename T, typename V>
T *
set(UinArray<T> &, std::size_t, V &&) noexcept;

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

template <typename T, typename V>
T *
exchange(UinArray<T> &, std::size_t, /*IN/OUT*/ V &) noexcept;

//=====================================
/*
 * Takes the in pointer and calculates a index based on the head of the array.
 * if the pointer is out of bounds[Array.buffer-Array.length] Array.capacity is
 * returned.
 */
template <typename T>
std::size_t
index_of(const Array<T> &, const T *) noexcept;

template <typename T>
std::size_t
index_of(const UinArray<T> &, const T *) noexcept;

//=====================================
template <typename T>
bool
remove(Array<T> &, std::size_t idx) noexcept;

template <typename T>
bool
remove(UinArray<T> &, std::size_t idx) noexcept;

//=====================================
template <typename T, typename Predicate>
void
remove_if(Array<T> &, Predicate) noexcept;

template <typename T, typename Predicate>
void
remove_if(UinArray<T> &, Predicate) noexcept;

//=====================================
template <typename T>
bool
stable_remove(Array<T> &, std::size_t idx) noexcept;

template <typename T>
bool
stable_remove(UinArray<T> &, std::size_t idx) noexcept;

//=====================================
/*
 * Take element at $index and swap it with $out, then swap():ed element at
 * $index again with the last element in the Array and decrement Array.length.
 */
template <typename T>
bool
take(Array<T> &, std::size_t, /*OUT*/ T &) noexcept;

template <typename T>
bool
take(UinArray<T> &, std::size_t, /*OUT*/ T &) noexcept;

//=====================================
/*
 *
 *
 */
template <typename T, typename Dest>
bool
stable_take(Array<T> &, std::size_t, /*OUT*/ Dest &) noexcept;

template <typename T, typename Dest>
bool
stable_take(UinArray<T> &, std::size_t, /*OUT*/ Dest &) noexcept;

//=====================================
template <typename T>
const T *
last(const Array<T> &) noexcept;

template <typename T>
T *
last(Array<T> &) noexcept;

template <typename T>
const T *
last(const UinArray<T> &) noexcept;

template <typename T>
T *
last(UinArray<T> &) noexcept;

//=====================================
template <typename Random, typename T>
void
shuffle(Random &, Array<T> &) noexcept;

template <typename Random, typename T>
void
shuffle(Random &, UinArray<T> &) noexcept;

//=====================================
template <typename T>
void
clear(Array<T> &) noexcept;

template <typename T>
void
clear(UinArray<T> &) noexcept;

//=====================================
template <typename T>
void
drop_back(Array<T> &, std::size_t) noexcept;

template <typename T>
void
drop_back(UinArray<T> &, std::size_t) noexcept;

//=====================================
template <typename T>
void
swap(Array<T> &, Array<T> &) noexcept;

template <typename T>
void
swap(UinArray<T> &, UinArray<T> &) noexcept;

//=====================================
template <typename T, typename F>
void
for_each(Array<T> &, F) noexcept;

template <typename T, typename F>
void
for_each(UinArray<T> &, F) noexcept;

template <typename T, typename F>
void
for_each(const Array<T> &, F) noexcept;

template <typename T, typename F>
void
for_each(const UinArray<T> &, F) noexcept;

//=====================================
template <typename T, typename F>
bool
for_all(Array<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(UinArray<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(const Array<T> &, F) noexcept;

template <typename T, typename F>
bool
for_all(const UinArray<T> &, F) noexcept;

//=====================================
template <typename T, typename Predicate>
T *
find(Array<T> &, Predicate) noexcept;

template <typename T, typename Predicate>
T *
find(UinArray<T> &, Predicate) noexcept;

template <typename T, typename Predicate>
const T *
find(const Array<T> &, Predicate) noexcept;

template <typename T, typename Predicate>
T *
find(const UinArray<T> &, Predicate) noexcept;

//=====================================
template <typename T, typename K, typename F>
K *
reduce(const Array<T> &, K *, F) noexcept;

template <typename T, typename K, typename F>
K *
reduce(const UinArray<T> &, K *, F) noexcept;

template <typename T, typename K, typename F>
K &
reduce(const Array<T> &, K &, F) noexcept;

template <typename T, typename K, typename F>
K &
reduce(const UinArray<T> &, K &, F) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
Array<T>::Array() noexcept
    : Array(nullptr, 0) {
}

template <typename T>
Array<T>::~Array() noexcept {
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
  // printf("Array(b:%p,len:%zu,cap:%zu)\n", this->buffer, this->length,
  //        this->capacity);
}

template <typename T>
Array<T>::Array(T *b, std::size_t l, std::size_t size) noexcept
    : buffer(b)
    , length(l)
    , capacity(size) {
}

template <typename T>
T &
Array<T>::operator[](std::size_t idx) noexcept {
  assertxs(buffer, idx, length, capacity);
  assertxs(idx < length, idx, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer[idx];
}

template <typename T>
const T &
Array<T>::operator[](std::size_t idx) const noexcept {
  assertxs(buffer, idx, length, capacity);
  assertxs(idx < length, idx, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer[idx];
}

template <typename T>
T *
Array<T>::data() noexcept {
  assertxs(buffer, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer;
}

template <typename T>
const T *
Array<T>::data() const noexcept {
  assertxs(buffer, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer;
}

template <typename T>
T *
Array<T>::begin() noexcept {
  return data();
}

template <typename T>
const T *
Array<T>::begin() const noexcept {
  return data();
}

template <typename T>
T *
Array<T>::end() noexcept {
  assertxs(buffer, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer + length;
}

template <typename T>
const T *
Array<T>::end() const noexcept {
  assertxs(buffer, length, capacity);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return buffer + length;
}

template <typename T>
bool
Array<T>::operator==(const Array &o) const noexcept {
  if (sp::length(o) != sp::length(*this)) {
    return false;
  }

  for (std::size_t i = 0; i < sp::length(o); ++i) {
    auto &s = this->operator[](i);
    if (!(s == o[i])) {
      return false;
    }
  }
  return true;
}

template <typename T>
template <typename T2, std::size_t SIZE>
bool
Array<T>::operator==(const T2 (&other)[SIZE]) const noexcept {
  if (SIZE != sp::length(*this)) {
    return false;
  }

  for (std::size_t i = 0; i < SIZE; ++i) {
    auto &s = this->operator[](i);
    if (!(s == other[i])) {
      return false;
    }
  }
  return true;
}

//=====================================
template <typename T>
DynamicArray<T>::DynamicArray(std::size_t cap) noexcept
    : Array<T>(nullptr, 0) {
  assertx(this->length == 0);
  assertx(this->capacity == 0);

  this->buffer = new T[cap]{};
  assertx(this->buffer);

  this->length = 0;
  assertx(this->length == 0);

  if (this->buffer) {
    this->capacity = cap;
    assertx(this->capacity == cap);
  }
}

template <typename T>
DynamicArray<T>::DynamicArray(std::initializer_list<T> l) noexcept
    : DynamicArray(l.size()) {
  assertx_n(insert_all(*this, l.begin(), l.size()));
}

template <typename T>
DynamicArray<T>::~DynamicArray() noexcept {
  if (this->buffer) {
    delete[] this->buffer;
  }
  this->buffer = nullptr;
  this->length = 0;
  this->capacity = 0;
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
  // init_list is compile time so all instances of init_list has a fixed size
  assertx(l.size() <= capacity(*this));
  assertx_n(insert_all(*this, l.begin(), l.size()));
}

//=====================================
template <typename T>
UinArray<T>::UinArray(storage_type *b, std::size_t c) noexcept
    : buffer{b}
    , capacity{c}
    , length{0} {

  if (!buffer) {
    assertxs(capacity == 0, capacity);
  }

  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);
}

template <typename T>
UinArray<T>::UinArray() noexcept
    : UinArray(nullptr, 0) {
}

template <typename T>
T *
UinArray<T>::data() noexcept {
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return reinterpret_cast<T *>(buffer);
}

template <typename T>
const T *
UinArray<T>::data() const noexcept {
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return reinterpret_cast<const T *>(buffer);
}

template <typename T>
T *
UinArray<T>::end() noexcept {
  return data() + length;
}

template <typename T>
const T *
UinArray<T>::end() const noexcept {
  return data() + length;
}

template <typename T>
T &
UinArray<T>::operator[](std::size_t idx) noexcept {
  assertxs(buffer, idx, length, capacity);
  assertxs(idx < length, idx, length);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return data()[idx];
}

template <typename T>
const T &
UinArray<T>::operator[](std::size_t idx) const noexcept {
  assertxs(buffer, idx, length, capacity);
  assertxs(idx < length, idx, length);
  assertxs(sp::length(*this) <= sp::capacity(*this), length, capacity);

  return data()[idx];
}

template <typename T>
bool
UinArray<T>::operator==(const UinArray &o) const noexcept {
  fprintf(stderr, "%s:0\n", __func__);
  if (sp::length(o) != sp::length(*this)) {
    return false;
  }

  for (std::size_t i = 0; i < sp::length(o); ++i) {
    auto &s = this->operator[](i);
    if (!(s == o[i])) {
      return false;
    }
  }
  return true;
}

template <typename T>
template <typename T2, std::size_t SIZE>
bool
UinArray<T>::operator==(const T2 (&other)[SIZE]) const noexcept {
  fprintf(stderr, "%s:1\n", __func__);
  if (SIZE != sp::length(*this)) {
    return false;
  }

  for (std::size_t i = 0; i < SIZE; ++i) {
    auto &s = this->operator[](i);
    if (!(s == other[i])) {
      return false;
    }
  }
  return true;
}

//=====================================
template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray() noexcept
    : UinArray<T>(raw, c)
/*TODO , raw{}*/ {
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray(UinStaticArray<T, c> &&o) noexcept
    : UinStaticArray() {

  for (std::size_t i = 0; i < length(o); ++i) {
    auto &current = o[i];
    assertx_n(insert(*this, std::move(current)));
    current.~T();
  }
  o.length = 0;
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::UinStaticArray(std::initializer_list<T> l) noexcept
    : UinStaticArray() {
  assertx(l.size() <= sp::capacity(*this));
  assertx_n(insert_all(*this, l.begin(), l.size()));
}

template <typename T, std::size_t c>
UinStaticArray<T, c>::~UinStaticArray() noexcept {
  if (this->buffer) {
    if (!std::is_trivially_destructible<T>::value) {
      for (std::size_t i = 0; i < length(*this); ++i) {
        auto &current = this->operator[](i);
        current.~T();
      }
    }
  }

  this->length = 0;
}

//=====================================
template <typename T>
UinDynamicArray<T>::UinDynamicArray(std::size_t cap) noexcept
    : UinArray<T>() {
  if (cap > 0) {
    using st = typename UinArray<T>::storage_type;

    this->buffer = new st[cap]{};
    if (this->buffer) {
      this->capacity = cap;
    }
  }
}

template <typename T>
UinDynamicArray<T>::UinDynamicArray(UinDynamicArray<T> &&l) noexcept
    : UinArray<T>() {
  swap(*this, l);
}

template <typename T>
UinDynamicArray<T>::UinDynamicArray(std::initializer_list<T> l) noexcept
    : UinDynamicArray(l.size()) {
  assertx_n(insert_all(*this, l.begin(), l.size()));
}

template <typename T>
UinDynamicArray<T>::~UinDynamicArray() noexcept {
  if (this->buffer) {
    if (!std::is_trivially_destructible<T>::value) {
      for (std::size_t i = 0; i < length(*this); ++i) {
        auto &current = this->operator[](i);
        current.~T();
      } // for
    }

    delete[] this->buffer;
  }

  this->buffer = nullptr;
  this->length = 0;
  this->capacity = 0;
}

//=====================================
template <typename T>
std::size_t
length(const Array<T> &self) noexcept {
  assertxs(self.length <= self.capacity, self.length, self.capacity);
  return self.length;
}

template <typename T>
std::size_t
length(const UinArray<T> &self) noexcept {
  assertxs(self.length <= self.capacity, self.length, self.capacity);

  return self.length;
}

//=====================================
template <typename T>
std::size_t
capacity(const Array<T> &self) noexcept {
  assertxs(self.length <= self.capacity, self.length, self.capacity);

  return self.capacity;
}

template <typename T>
std::size_t
capacity(const UinArray<T> &self) noexcept {
  assertxs(self.length <= self.capacity, self.length, self.capacity);

  return self.capacity;
}

//=====================================
template <typename T>
std::size_t
remaining_write(const Array<T> &self) noexcept {
  return capacity(self) - length(self);
}

template <typename T>
std::size_t
remaining_write(const UinArray<T> &self) noexcept {
  return capacity(self) - length(self);
}

//=====================================
template <typename T>
bool
is_empty(const Array<T> &self) noexcept {
  return length(self) == 0;
}

template <typename T>
bool
is_empty(const UinArray<T> &self) noexcept {
  return length(self) == 0;
}

//=====================================
template <typename T>
bool
is_full(const Array<T> &self) noexcept {
  return length(self) == capacity(self);
}

template <typename T>
bool
is_full(const UinArray<T> &self) noexcept {
  return length(self) == capacity(self);
}

//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert(Array<T> &self, V &&val, Comparator &cmp) noexcept {
  return bin_insert(self, std::forward<V>(val), cmp);
}

template <typename T, typename V, typename Comparator>
T *
bin_insert(Array<T> &self, V &&val) noexcept {
  Comparator cmp;
  return bin_insert(self, std::forward<V>(val), cmp);
}

template <typename T, typename K, typename Comparator>
static const T *
bin_find_gte(const UinArray<T> &self, const K &needle,
             Comparator &cmp) noexcept {
  if (is_empty(self)) {
    return nullptr;
  }

  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  std::size_t length = sp::length(self);
  const T *it = self.data();
  const T *const end = it + length;
Lit : {
  const std::size_t mid = middle(length);
  if (cmp(it[mid], /*>*/ needle)) {
    if (length == 0) {
      return it;
    }

    length = mid;

    goto Lit;
  } else if (cmp(needle, /*>*/ it[mid])) {
    if (length == 1) {
      if (it + length != end) {
        return it + length;
      } else {
        return nullptr;
      }
    }

    it = it + mid;
    length = length - mid;

    goto Lit;
  } else {
    /* needle is exact match */
    return it + mid;
  }
}

  return nullptr;
}

template <typename T, typename K, typename Cmp>
static T *
bin_find_gte(UinArray<T> &self, const K &needle, Cmp &cmp) noexcept {
  const auto &c_self = self;
  return (T *)bin_find_gte(c_self, needle, cmp);
}

template <typename T, typename V, typename Comparator>
T *
bin_insert(UinArray<T> &self, V &&val, Comparator &cmp) noexcept {
  if (!is_full(self)) {
    T *const first = bin_find_gte(/*>=*/self, val, cmp);
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

template <typename T, typename V, typename Comparator>
T *
bin_insert(UinArray<T> &self, V &&val) noexcept {
  Comparator cmp;
  return bin_insert(self, val, cmp);
}

//=====================================
template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(Array<T> &, V &&, Comparator &cmp) noexcept {
  assertx(false);
  return nullptr;
}

template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(Array<T> &self, V &&val) noexcept {
  Comparator cmp;
  return bin_insert_unique(self, std::forward<V>(val), cmp);
}

template <typename T, typename V, typename C>
T *
bin_insert_unique(UinArray<T> &self, V &&val, C &cmp) noexcept {
  if (is_full(self)) {
    return nullptr;
  }

  T *const first = bin_find_gte(/*>=*/self, val, cmp);
  if (first) {
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

template <typename T, typename V, typename Comparator>
T *
bin_insert_unique(UinArray<T> &self, V &&val) noexcept {
  Comparator cmp;
  return bin_insert_unique(self, std::forward<V>(val), cmp);
}

//=====================================
template <typename T, typename K, typename Comparator>
const T *
bin_search(const Array<T> &self, const K &needle, Comparator &cmp) noexcept {
  auto middle = [](std::size_t len) {
    /**/
    return len / 2;
  };

  if (length(self) > 0) {

    std::size_t length = sp::length(self);
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
const T *
bin_search(const Array<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return bin_search(self, needle, cmp);
}

template <typename T, typename K, typename Comparator>
T *
bin_search(Array<T> &self, const K &needle, Comparator &cmp) noexcept {
  const Array<T> &c_self = self;
  return (T *)bin_search(c_self, needle, cmp);
}

template <typename T, typename K, typename Comparator>
T *
bin_search(Array<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return bin_search(self, needle, cmp);
}

template <typename T, typename K, typename C>
const T *
bin_search(const UinArray<T> &self, const K &needle, C &cmp) noexcept {
  const Array<T> c_self((T *)self.data(), length(self), capacity(self));
  return bin_search(c_self, needle, cmp);
}

template <typename T, typename K, typename Comparator>
const T *
bin_search(const UinArray<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return bin_search(self, needle, cmp);
}

template <typename T, typename K, typename Cmp>
T *
bin_search(UinArray<T> &self, const K &needle, Cmp &cmp) noexcept {
  const auto &c_self = self;
  return (T *)bin_search(c_self, needle, cmp);
}

template <typename T, typename K, typename Comparator>
T *
bin_search(UinArray<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return (T *)bin_search(self, needle, cmp);
}

//=====================================
template <typename T, typename K, typename Cmp>
bool
bin_remove(UinArray<T> &self, const K &needle, Cmp &cmp) noexcept {
  T *const res = bin_search(self, needle, cmp);
  if (res) { // TODO maybe stable_remove?
    std::size_t rem_idx = index_of(self, res);
    for (; (rem_idx + 1) < length(self);) {
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
bin_remove(UinArray<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return bin_remove(self, needle, cmp);
}

template <typename T, typename K, typename Comparator>
bool
bin_remove(Array<T> &, const K &, Comparator &) noexcept {
  assertx(false);
  return false;
}

template <typename T, typename K, typename Comparator>
bool
bin_remove(Array<T> &self, const K &needle) noexcept {
  Comparator cmp;
  return bin_remove(self, needle, cmp);
}

//=====================================
template <typename T>
T *
get(Array<T> &self, std::size_t idx) noexcept {
  const auto &c_self = self;
  return (T *)get(c_self, idx);
}

template <typename T>
T *
get(UinArray<T> &self, std::size_t idx) noexcept {
  const auto &c_self = self;
  return (T *)get(c_self, idx);
}

template <typename T>
const T *
get(const Array<T> &self, std::size_t idx) noexcept {
  if (idx < length(self)) {
    return self.buffer + idx;
  }

  // assertxs(idx < length(self), idx, length(self), capacity(self));
  return nullptr;
}

template <typename T>
const T *
get(const UinArray<T> &self, std::size_t idx) noexcept {
  const Array<T> c_self((T *)self.data(), length(self), capacity(self));
  return get(c_self, idx);
}
//=====================================
template <typename T, typename V>
T *
insert(Array<T> &self, V &&val) noexcept {
  if (!is_full(self)) {
    const std::size_t idx = self.length++;
    T *const raw = self.buffer + idx;

    // dtor of default init T
    raw->~T();
    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, typename V>
T *
insert(UinArray<T> &self, V &&val) noexcept {
  if (!is_full(self)) {
    const std::size_t idx = self.length++;
    T *const raw = self.data() + idx;

    return new (raw) T(std::forward<V>(val));
  }

  return nullptr;
}
//=====================================

template <typename T, typename From>
bool
insert_all(Array<T> &self, const From &src) noexcept {
  const std::size_t len = length(src);
  if (remaining_write(self) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      assertx_n(insert(self, src[i]));
    }

    return true;
  }

  return false;
}

template <typename T, typename From>
bool
insert_all(UinArray<T> &self, const From &src) noexcept {
  const std::size_t len = length(src);
  if (remaining_write(self) >= len) {
    for (std::size_t i = 0; i < len; ++i) {
      T *res = insert(self, src[i]);
      assertx(res);
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
      T *res = insert(self, values[i]);
      assertx(res);
    }

    return true;
  }

  return false;
}

template <typename T, typename V>
bool
insert_all(UinArray<T> &self, const V *values, std::size_t length) noexcept {
  if (remaining_write(self) >= length) {
    for (std::size_t i = 0; i < length; ++i) {
      T *res = insert(self, values[i]);
      assertx(res);
    }

    return true;
  }

  return false;
}

//=====================================
template <typename T>
bool
move_all(UinArray<T> &self, Array<T> &other) noexcept {
  if (remaining_write(self) >= length(other)) {
    for (std::size_t i = 0; i < length(other); ++i) {
      T *res = insert(self, std::move(other[i]));
      assertx(res);
    }

    clear(other);
    return true;
  }

  return false;
}

template <typename T>
bool
move_all(UinArray<T> &self, UinArray<T> &other) noexcept {
  if (remaining_write(self) >= length(other)) {
    for (std::size_t i = 0; i < length(other); ++i) {
      assertx_n(insert(self, std::move(other[i])));
    }

    clear(other);
    return true;
  }

  return false;
}

//=====================================
template <typename T, typename V>
T *
push(Array<T> &self, V &&value) noexcept {
  return insert(self, std::forward<V>(value));
}

template <typename T, typename V>
T *
push(UinArray<T> &self, V &&value) noexcept {
  return insert(self, std::forward<V>(value));
}

//=====================================
template <typename T>
bool
pop(Array<T> &self) noexcept {
  if (!is_empty(self)) {
    drop_back(self, 1);
    return true;
  }
  return false;
}

template <typename T>
bool
pop(UinArray<T> &self) noexcept {
  if (!is_empty(self)) {
    drop_back(self, 1);
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

template <typename T, typename V>
T *
insert_at(UinArray<T> &self, std::size_t idx, V &&v) noexcept {
  if (is_full(self)) {
    return nullptr;
  }

  if (idx <= length(self)) {
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

template <typename T, typename V>
T *
set(UinArray<T> &self, std::size_t idx, V &&val) noexcept {
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
  if (idx < length(self)) {
    using std::swap;
    swap(self.buffer[idx], out);
    return self.buffer + idx;
  }
  return nullptr;
}

template <typename T, typename V>
T *
exchange(UinArray<T> &self, std::size_t idx,
         /*IN/OUT*/ V &out) noexcept {
  Array<T> c_self(self.data(), length(self), capacity(self));
  return exchange(c_self, idx, out);
}

//=====================================
template <typename T>
std::size_t
index_of(const Array<T> &self, const T *ptr) noexcept {
  // util/array.h
  return sp::index_of(self.buffer, length(self), capacity(self), ptr);
}

template <typename T>
std::size_t
index_of(const UinArray<T> &self, const T *ptr) noexcept {
  const Array<T> c_self((T *)self.data(), length(self), capacity(self));
  return index_of(c_self, ptr);
}

//=====================================
template <typename T>
bool
remove(Array<T> &self, std::size_t idx) noexcept {
  assertxs(idx < length(self), idx, length(self));

  if (idx < length(self)) {
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

template <typename T>
bool
remove(UinArray<T> &self, std::size_t idx) noexcept {
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

template <typename T, typename Predicate>
void
remove_if(UinArray<T> &self, Predicate p) noexcept {
  T *it = self.data();
  while (it != self.end()) {
    const bool del = p(*it);
    if (del) {
      std::size_t index = index_of(self, it);
      assertxs(index != capacity(self), length(self), capacity(self));

      bool res = remove(self, index);
      assertx(res);
    } else {
      ++it;
    }
  }
}

//=====================================
template <typename T>
bool
stable_remove(Array<T> &, std::size_t idx) noexcept {
  assertx(false);
  return false;
}

template <typename T>
bool
stable_remove(UinArray<T> &self, std::size_t idx) noexcept {
  if (idx < length(self)) {

    T *it = self.data() + idx;
    T *next = it + 1;
    while (next != self.end()) {
      using std::swap;
      swap(*it, *next);
      it = next;
      next = it + 1;
    }

    it->~T();
    --self.length;

    return true;
  }

  return false;
}

//=====================================
template <typename T>
bool
take(Array<T> &self, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < length(self)) {
    using std::swap;
    swap(out, self.buffer[idx]);

    bool res = remove(self, idx);
    assertx(res);

    return true;
  }

  return false;
}

template <typename T>
bool
take(UinArray<T> &self, std::size_t idx, /*OUT*/ T &out) noexcept {
  if (idx < length(self)) {
    using std::swap;
    swap(out, self.data()[idx]);

    bool res = remove(self, idx);
    assertx(res);

    return true;
  }

  return false;
}
//=====================================
template <typename T, typename Dest>
bool
stable_take(Array<T> &self, std::size_t idx, /*OUT*/ Dest &out) noexcept {
  if (idx < length(self)) {
    out = std::move(self.data()[idx]);

    bool res = stable_remove(self, idx);
    assertx(res);

    return true;
  }

  return false;
}

template <typename T, typename Dest>
bool
stable_take(UinArray<T> &self, std::size_t idx, Dest &out) noexcept {
  if (idx < length(self)) {
    out = std::move(self.data()[idx]);

    bool res = stable_remove(self, idx);
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

template <typename T>
const T *
last(const UinArray<T> &self) noexcept {
  if (is_empty(self)) {
    return nullptr;
  }

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

template <typename T>
T *
last(UinArray<T> &self) noexcept {
  if (is_empty(self)) {
    return nullptr;
  }

  std::size_t idx = length(self) - 1;
  return get(self, idx);
}

//=====================================
template <typename Random, typename T>
void
shuffle(Random &r, Array<T> &self) noexcept {
  for (std::size_t idx = 0; idx < length(self); ++idx) {
    auto replace =
        uniform_dist(r, std::uint32_t(0), std::uint32_t(self.length));
    if (idx != replace) {
      using std::swap;
      swap(self.buffer[idx], self.buffer[replace]);
    }
  }
}

template <typename Random, typename T>
void
shuffle(Random &r, UinArray<T> &self) noexcept {
  Array<T> c_self(self.data(), length(self), capacity(self));
  return shuffle(r, c_self);
}

//=====================================
template <typename T>
void
clear(Array<T> &self) noexcept {
  drop_back(self, length(self));
  assertx(is_empty(self));
}

template <typename T>
void
clear(UinArray<T> &self) noexcept {
  drop_back(self, length(self));
  assertx(is_empty(self));
}

//=====================================
template <typename T>
void
drop_back(Array<T> &self, std::size_t len) noexcept {
  assertxs(len <= length(self), len, length(self));

  len = std::min(length(self), len);
  for (std::size_t idx = len; idx-- > 0;) {
    T *const p = self.buffer + idx;
    p->~T();
  }
  self.length -= len;
}

template <typename T>
void
drop_back(UinArray<T> &self, std::size_t len) noexcept {
  assertxs(len <= length(self), len, length(self));

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

template <typename T>
void
swap(UinArray<T> &f, UinArray<T> &s) noexcept {
  swap(f.buffer, s.buffer);
  swap(f.length, s.length);
  swap(f.capacity, s.capacity);
}

//=====================================
template <typename T, typename F>
void
for_each(Array<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    f(self.buffer[i]);
  }
}

template <typename T, typename F>
void
for_each(UinArray<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    f(self.data()[i]);
  }
}

template <typename T, typename F>
void
for_each(const Array<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    const T &current = self.buffer[i];
    f(current);
  }
}

template <typename T, typename F>
void
for_each(const UinArray<T> &self, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    const T &current = self.data()[i];
    f(current);
  }
}

//=====================================
template <typename T, typename F>
bool
for_all(Array<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < length(self) && result; ++i) {
    result = f(self.buffer[i]);
  }
  return result;
}
template <typename T, typename F>
bool
for_all(UinArray<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < length(self) && result; ++i) {
    result = f(self.data()[i]);
  }
  return result;
}

template <typename T, typename F>
bool
for_all(const Array<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < length(self) && result; ++i) {
    const T &current = self.buffer[i];
    result = f(current);
  }
  return result;
}

template <typename T, typename F>
bool
for_all(const UinArray<T> &self, F f) noexcept {
  bool result = true;
  for (std::size_t i = 0; i < length(self) && result; ++i) {
    const T &current = self.data()[i];
    result = f(current);
  }
  return result;
}

//=====================================
template <typename T, typename Predicate>
T *
find(Array<T> &self, Predicate f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    T &current = self.buffer[i];
    if (f(current)) {
      return self.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, typename Predicate>
T *
find(UinArray<T> &self, Predicate f) noexcept {
  Array<T> ar(self.data(), length(self), self.capacity);
  return find(ar, f);
}

template <typename T, typename Predicate>
const T *
find(const Array<T> &self, Predicate f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    const auto &current = self.buffer[i];
    if (f(current)) {
      return self.buffer + i;
    }
  }
  return nullptr;
}

template <typename T, typename Predicate>
T *
find(const UinArray<T> &self, Predicate f) noexcept {
  const Array<T> ar(self.data(), length(self), self.capacity);
  return find(ar, f);
}

//=====================================
template <typename T, typename K, typename F>
K *
reduce(const Array<T> &self, K *k, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    const T *current = self.buffer + i;
    k = f(k, current);
  }

  return k;
}

template <typename T, typename K, typename F>
K *
reduce(const UinArray<T> &self, K *k, F f) noexcept {
  const Array<T> ar((T *)self.data(), length(self), self.capacity);
  return reduce(ar, k, f);
}

template <typename T, typename K, typename F>
K &
reduce(const Array<T> &self, K &k, F f) noexcept {
  for (std::size_t i = 0; i < length(self); ++i) {
    const T &current = self.buffer[i];
    k = f(k, current);
  }

  return k;
}

template <typename T, typename K, typename F>
K &
reduce(const UinArray<T> &self, K &k, F f) noexcept {
  const Array<T> ar((T *)self.data(), length(self), self.capacity);
  return reduce(ar, k, f);
}

} // namespace sp

#endif
