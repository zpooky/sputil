#ifndef SP_UTIL_MAP_HASH_SET_OPEN_ADDR_H
#define SP_UTIL_MAP_HASH_SET_OPEN_ADDR_H

#include <cmath>
#include <cstddef>
#include <hash/standard.h>
#include <type_traits>
#include <util/assert.h>
#include <utility>

namespace sp {
namespace impl {
template <typename T>
struct HashSetOpenBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  st value;
  bool present;

  HashSetOpenBucket() noexcept;
};

} // namespace impl

//=====================================
template <typename T, typename Hash = sp::Hasher<T>, typename Eq = sp::Equality>
struct HashSetOpen {
  impl::HashSetOpenBucket<T> *items;
  std::size_t capacity;
  std::size_t length;

  HashSetOpen(std::size_t cap = 10) noexcept;
  ~HashSetOpen() noexcept;
};

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetOpen<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSetOpen<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetOpen<T, H, Eq> &, const V &) noexcept;

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSetOpen<T, H, Eq> &, const V &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
HashSetOpenBucket<T>::HashSetOpenBucket() noexcept
    : value{}
    , present{false} {
}

} // namespace impl

//=====================================
template <typename T, typename Hash, typename Eq>
HashSetOpen<T, Hash, Eq>::HashSetOpen(std::size_t cap) noexcept
    : items{nullptr}
    , capacity{cap}
    , length{0} {
}

template <typename T, typename Hash, typename Eq>
HashSetOpen<T, Hash, Eq>::~HashSetOpen() noexcept {
  if (this->items) {
    delete[] this->items;
  }
  this->items = nullptr;
  this->capacity = 0;
  this->length = 0;
}

//=====================================
namespace impl {
template <typename T>
static std::size_t
ht_hash(const T &key, const std::size_t a, const std::size_t m) {
  std::size_t hash = 0;
  const unsigned char *s = (unsigned char *)&key;
  const std::size_t len_s = sizeof(key);

  for (std::size_t i = 0; i < len_s; i++) {
    hash += std::pow(a, len_s - (i + 1)) * s[i];
    hash = hash % m;
  }
  return hash;
}

template <typename T>
static std::size_t
get_hash(const T &key, const std::size_t buckets, const std::size_t attempt) {
  const int hash_a = ht_hash(key, 9, buckets);
  const int hash_b = ht_hash(key, 11, buckets);
  return (hash_a + (attempt * (hash_b + 1))) % buckets;
  // return 0;
}

template <typename T, typename H, typename Eq, typename V>
static const HashSetOpenBucket<T> *
find_lookup(const HashSetOpen<T, H, Eq> &self, const V &value) noexcept {
  return nullptr;
}

template <typename T, typename H, typename Eq, typename V>
static HashSetOpenBucket<T> *
find_dd(HashSetOpen<T, H, Eq> &self, const V &value) noexcept {
  HashSetOpenBucket<T> *current = nullptr;
  std::size_t i = 0;

  do {
    std::size_t index;
    index = get_hash(value, self.capacity, i);
    current = self.items + index;

    if (current->present) {
      const T *current_val = (T *)&current->value;
      Eq equal;

      if (equal(*current_val, value)) {
        return current;
      }
    }

    i++;
  } while (current->present && i < self.capacity);

  if (current->present) {
    return nullptr;
  }

  return current;
}

} // namespace impl

// https://github.com/jamesroutley/write-a-hash-table/tree/master/05-methods
template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetOpen<T, H, Eq> &self, V &&value) noexcept {
  using namespace impl;

  if (!self.items) {
    self.items = new HashSetOpenBucket<T>[self.capacity];
  }

Lretry:
  T *result = nullptr;
  if (self.items) {
    HashSetOpenBucket<T> *current = find_dd(self, value);

    if (!current) {
      // TODO rehash();
    } else if (!current->present) {
      result = new (&current->value) T{std::forward<V>(value)};
      current->present = true;

      self.length++;
    }
  }

  return result;
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSetOpen<T, H, Eq> &, V &&) noexcept {
  return nullptr;
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetOpen<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;

  const T *result = nullptr;
  if (self.items) {
    const HashSetOpenBucket<T> *current = find_lookup(self, needle);
    if (current) {
      assertx(current->present);
      result = (const T *)&current->value;
    }
  }

  return result;
}

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSetOpen<T, H, Eq> &self, const V &needle) noexcept {
  const auto &c_self = self;
  return (T *)lookup(c_self, needle);
}

} // namespace sp

#endif
