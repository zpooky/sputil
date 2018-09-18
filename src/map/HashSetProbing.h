#ifndef SP_UTIL_MAP_HASH_SET_PROBING_H
#define SP_UTIL_MAP_HASH_SET_PROBING_H

#include <algorithm>
#include <cstddef>
#include <hash/standard.h>
#include <type_traits>
#include <util/array.h>
#include <util/assert.h>

namespace sp {
//=====================================
namespace impl {
enum class HSPTag : int { PRESENT, EMPTY, TOMBSTONE };

template <typename T>
struct HashSetProbingBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

  st value;
  HSPTag tag;

  HashSetProbingBucket() noexcept;

  ~HashSetProbingBucket() noexcept;
};
}

//=====================================
template <typename T, typename Hash = sp::Hasher<T>, typename Eq = sp::Equality>
struct HashSetProbing {
  impl::HashSetProbingBucket<T> *table;
  std::size_t capacity;

  HashSetProbing(std::size_t cap = 16) noexcept;

  ~HashSetProbing() noexcept;
};

//=====================================
/* returns null on duplicate
 */
template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetProbing<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSetProbing<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetProbing<T, H, Eq> &, const V &) noexcept;

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSetProbing<T, H, Eq> &, const V &) noexcept;

/*
 * //=====================================
 * template <typename T, typename H, typename Eq>
 * const T *
 * lookup_default(const HashSetProbing<T, H, Eq> &, const T &needle,
 *                const T &def) noexcept;
 *
 * template <typename T, typename H, typename Eq>
 * T *
 * lookup_default(HashSetProbing<T, H, Eq> &, const T &needle, T &def) noexcept;
 *
 * //=====================================
 * template <typename T, typename H, typename Eq, typename V>
 * T *
 * lookup_insert(HashSetProbing<T, H, Eq> &, V &&) noexcept;
 *
 * //=====================================
 * template <typename T, typename H, typename Eq, typename V, typename Compute>
 * T *
 * lookup_compute(HashSetProbing<T, H, Eq> &, const V &needle, Compute)
 * noexcept;
 *
 * //=====================================
 * template <typename T, typename H, typename Eq, typename V>
 * bool
 * contains(const HashSetProbing<T, H, Eq> &, const V &) noexcept;
 */

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSetProbing<T, H, Eq> &, const V &) noexcept;

//=====================================
template <typename T, typename H, typename Eq>
std::size_t
capacity(const HashSetProbing<T, H, Eq> &) noexcept;

//=====================================
template <typename T, typename H, typename Eq>
void
swap(HashSetProbing<T, H, Eq> &, HashSetProbing<T, H, Eq> &) noexcept;

//=====================================
namespace rec {
template <typename T, typename H, typename Eq>
bool
verify(const HashSetProbing<T, H, Eq> &) noexcept;
}

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
HashSetProbingBucket<T>::HashSetProbingBucket() noexcept
    : value{}
    , tag{HSPTag::EMPTY} {
}

template <typename T>
HashSetProbingBucket<T>::~HashSetProbingBucket() noexcept {
  if (this->tag == HSPTag::PRESENT) {
    T *v = (T *)&this->value;
    v->~T();
  }
  this->tag = HSPTag::EMPTY;
}
}

template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::HashSetProbing(std::size_t cap) noexcept
    : table{nullptr}
    , capacity{cap < 16 ? 16 : cap} {
}

template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::~HashSetProbing() noexcept {
  if (this->table) {
    delete[] this->table;
  }

  this->table = nullptr;
  this->capacity = 0;
}

//=====================================
namespace impl {
static inline std::size_t
index_of(std::size_t hash, std::size_t length) noexcept {
  const std::size_t res = hash & (length - 1);
  const std::size_t cmp = hash % length;
  assertxs(res < length, res, length);
  assertxs(cmp == res, hash, length, cmp, res);
  return res;
}

template <typename T, typename H, typename Eq>
static bool
rehash(HashSetProbing<T, H, Eq> &self) noexcept {
  HashSetProbing<T, H, Eq> tmp;
  tmp.capacity = self.capacity * 2;
  tmp.table = new impl::HashSetProbingBucket<T>[tmp.capacity];

  if (tmp.table) {
    for (std::size_t i = 0; i < capacity(self); ++i) {
      auto &bucket = self.table[i];
      if (bucket.tag == HSPTag::PRESENT) {
        bucket.tag = HSPTag::TOMBSTONE;
        T *const current = (T *)&bucket.value;

        assertx_n(insert(tmp, std::move(*current)));
      }
    }

    swap(self, tmp);
    return true;
  }

  return false;
}
}

template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetProbing<T, H, Eq> &self, V &&value) noexcept {
  using namespace impl;

  if (!self.table) {
    self.table = new impl::HashSetProbingBucket<T>[self.capacity];
  }

Lretry:
  if (self.table) {
    H hash;
    const std::size_t h = hash(value);
    const std::size_t dest = index_of(h, self.capacity);
    std::size_t idx = dest;

    do {
      auto &bucket = self.table[idx];
      T *const res = (T *)&bucket.value;

      if (bucket.tag != HSPTag::PRESENT) {

        bucket.tag = HSPTag::PRESENT;
        return new (res) T(std::forward<V>(value));
      } else {
        Eq equality;
        const V &needle = value;

        if (equality(*res, needle)) {
          /* Duplicate */
          return nullptr;
        }
      }

      idx = (idx + 1) % self.capacity;
    } while (idx != dest);

    if (rehash(self)) {
      goto Lretry;
    }
  }

  return nullptr;
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSetProbing<T, H, Eq> &, V &&) noexcept {
  assertx(false);
  return nullptr;
}

//=====================================
namespace impl {
template <typename T, typename H, typename Eq, typename V>
static const HashSetProbingBucket<T> *
lookup_bucket(const HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  if (self.table) {
    H hash;
    const std::size_t h = hash(needle);
    const std::size_t dest = impl::index_of(h, self.capacity);
    std::size_t idx = dest;

    do {
      auto &bucket = self.table[idx];
      const T *const res = (T *)&bucket.value;

      if (bucket.tag == impl::HSPTag::PRESENT) {

        Eq equality;
        if (equality(*res, needle)) {
          /* Match */
          return &bucket;
        }
      } else if (bucket.tag == impl::HSPTag::EMPTY) {
        break;
      }

      idx = (idx + 1) % self.capacity;
    } while (idx != dest);
  }

  return nullptr;
}

template <typename T, typename H, typename Eq, typename V>
static HashSetProbingBucket<T> *
lookup_bucket(HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  const auto &c_self = self;
  return (HashSetProbingBucket<T> *)lookup_bucket(c_self, needle);
}
}

template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;

  const HashSetProbingBucket<T> *bucket = lookup_bucket(self, needle);
  if (bucket) {
    return (const T *)&bucket->value;
  }

  return nullptr;
}

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  const auto &c_self = self;
  return (T *)lookup(c_self, needle);
}

//=====================================
namespace impl {
template <typename T, typename H, typename Eq>
static void
cleanup(HashSetProbing<T, H, Eq> &self, HashSetProbingBucket<T> *s) noexcept {
  assertx(s);

  const std::size_t capacity = self.capacity;
  const std::size_t dest = sp::index_of(self.table, capacity, capacity, s);

  assertxs(dest != capacity, dest, capacity);
  auto &table = self.table;

  const std::size_t next_idx = (dest + 1) % capacity;
  if (table[next_idx].tag != HSPTag::EMPTY) {
    return;
  }

  std::size_t idx = dest;
  do {
    if (table[idx].tag == HSPTag::PRESENT) {
      return;
    }
    assertx(table[idx].tag == HSPTag::EMPTY ||
            table[idx].tag == HSPTag::TOMBSTONE);

    table[idx].tag = HSPTag::EMPTY;

    idx = (idx - 1) % capacity;
  } while (idx != dest);
}
}

template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;

  HashSetProbingBucket<T> *bucket = lookup_bucket(self, needle);
  if (bucket) {
    bucket->tag = impl::HSPTag::TOMBSTONE;
    T *const value = (T *)&bucket->value;
    value->~T();

    // cleanup(self, bucket);
    return true;
  }

  return false;
}

//=====================================
template <typename T, typename H, typename Eq>
std::size_t
capacity(const HashSetProbing<T, H, Eq> &self) noexcept {
  if (!self.table) {
    return 0;
  }

  return self.capacity;
}

//=====================================
template <typename T, typename H, typename Eq>
void
swap(HashSetProbing<T, H, Eq> &f, HashSetProbing<T, H, Eq> &s) noexcept {
  using std::swap;
  swap(f.table, s.table);
  swap(f.capacity, s.capacity);
}

//=====================================
namespace rec {
template <typename T, typename H, typename Eq>
bool
verify(const HashSetProbing<T, H, Eq> &) noexcept {
  return true;
}
}

//=====================================
}

#endif
