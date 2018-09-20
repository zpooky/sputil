#ifndef SP_UTIL_MAP_HASH_SET_PROBING_H
#define SP_UTIL_MAP_HASH_SET_PROBING_H

#include <algorithm>
#include <cstddef>
#include <hash/standard.h>
#include <type_traits>
#include <util/Quadset.h>
#include <util/array.h>
#include <util/assert.h>

#define HSPTag_EMPTY char(0)
#define HSPTag_PRESENT char(1)
#define HSPTag_TOMBSTONE char(2)

namespace sp {
namespace impl {
//=====================================
template <typename T>
struct HashSetProbingBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

  st value;
  // HSPTag tag;

  HashSetProbingBucket() noexcept;

  ~HashSetProbingBucket() noexcept;
};
}

//=====================================
template <typename T, typename Hash = sp::Hasher<T>, typename Eq = sp::Equality>
struct HashSetProbing {
  impl::HashSetProbingBucket<T> *table;
  sp::Quadset tags;

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
template <typename T, typename H, typename Eq, typename F>
void
for_each(HashSetProbing<T, H, Eq> &, F) noexcept;

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
    : value{} {
}

template <typename T>
HashSetProbingBucket<T>::~HashSetProbingBucket() noexcept {
}
}

template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::HashSetProbing(std::size_t cap) noexcept
    : table{nullptr}
    , tags{nullptr, 0} // TODO
    , capacity{cap < 16 ? 16 : cap} {
}

template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::~HashSetProbing() noexcept {
  if (this->table) {
    for (std::size_t idx = 0; idx < this->capacity; ++idx) {
      auto &current = this->table[idx];
      const auto tag = test(this->tags, idx);

      if (tag == HSPTag_PRESENT) {
        T *v = (T *)&current.value;
        v->~T();
      }
    }

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
  // printf("rehash()\n");
  HashSetProbing<T, H, Eq> tmp;
  tmp.capacity = self.capacity * 2;
  // XXX new and check non null before move

  std::size_t cnt = 0;
  for (std::size_t idx = 0; idx < capacity(self); ++idx) {
    auto &bucket = self.table[idx];
    const auto tag = sp::test(self.tags, idx);

    if (tag == HSPTag_PRESENT) {
      // printf("cnt[%zu]\n", cnt);
      ++cnt;
      sp::set(self.tags, idx, HSPTag_TOMBSTONE);
      T *const current = (T *)&bucket.value;

      assertx_n(insert(tmp, std::move(*current)));
      current->~T();
    } else {
      // only rehash when full, for now...
      assertx(false);
    }
  }

  // printf("----\n");
  swap(self, tmp);

  return true;
}
}

template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetProbing<T, H, Eq> &self, V &&value) noexcept {
  // printf("insert()\n");
  using namespace impl;

  if (!self.table) {
    // XXX check null
    assertxs(self.capacity > 0, self.capacity);
    self.table = new impl::HashSetProbingBucket<T>[self.capacity];

    self.tags.capacity = Quadset_number_of_buffer(self.capacity);
    assertxs(self.tags.capacity > 0, self.tags.capacity, self.capacity);
    self.tags.buffer = new std::uint64_t[self.tags.capacity]{0};
  }

Lretry:
  if (self.table) {
    H hash;
    const std::size_t h = hash(value);
    const std::size_t dest = index_of(h, self.capacity);
    std::size_t idx = dest;

    std::size_t empty = self.capacity;
    do {
      auto &bucket = self.table[idx];
      const auto tag = sp::test(self.tags, idx);

      if (empty == self.capacity) {
        if (tag == HSPTag_EMPTY || tag == HSPTag_TOMBSTONE) {
          empty = idx;
        }
      }

      if (tag == HSPTag_EMPTY) {
        break;
      } else if (tag == HSPTag_PRESENT) {
        Eq equality;

        T *const res = (T *)&bucket.value;
        const V &needle = value;

        if (equality(*res, needle)) {
          /* Duplicate */
          return nullptr;
        }
      }

      idx = (idx + 1) % self.capacity;
    } while (idx != dest);

    if (empty != self.capacity) {
      assertx(empty < self.capacity);
      sp::set(self.tags, empty, HSPTag_PRESENT);

      T *const res = (T *)&self.table[empty].value;
      return new (res) T(std::forward<V>(value));
    }

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
static std::size_t
lookup_bucket(const HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  if (self.table) {
    H hash;
    const std::size_t h = hash(needle);
    const std::size_t dest = impl::index_of(h, self.capacity);
    std::size_t idx = dest;

    do {
      auto &bucket = self.table[idx];
      const auto tag = sp::test(self.tags, idx);
      const T *const res = (T *)&bucket.value;

      if (tag == HSPTag_PRESENT) {

        Eq equality;
        if (equality(*res, needle)) {
          /* Match */
          return idx;
        }
      } else if (tag == HSPTag_EMPTY) {
        break;
      }

      idx = (idx + 1) % self.capacity;
    } while (idx != dest);
  }

  return self.capacity;
}
}

template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;
  // printf("lookup()\n");

  std::size_t index = lookup_bucket(self, needle);
  if (index != self.capacity) {
    const auto &bucket = self.table[index];
    return (const T *)&bucket.value;
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
cleanup(HashSetProbing<T, H, Eq> &self,
        HashSetProbingBucket<T> *bucket) noexcept {
  assertx(bucket);

  const std::size_t capacity = self.capacity;
  const std::size_t dest = sp::index_of(self.table, capacity, capacity, bucket);

  assertxs(dest != capacity, dest, capacity);
  auto &table = self.table;

  const std::size_t next_idx = (dest + 1) % capacity;
  auto tag = sp::test(self.tags, next_idx);
  if (tag != HSPTag_EMPTY) {
    return;
  }

  std::size_t idx = dest;
  do {
    const auto tag = sp::test(self.tags, idx);
    if (tag == HSPTag_PRESENT) {
      return;
    }

    assertx(tag == HSPTag_EMPTY || tag == HSPTag_TOMBSTONE);
    sp::set(self.tags, idx, HSPTag_EMPTY);

    idx = (idx - 1) % capacity;
  } while (idx != dest);
}
}

template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;
  // printf("remve()\n");

  std::size_t index = lookup_bucket(self, needle);
  if (index != self.capacity) {
    auto &bucket = self.table[index];
    sp::set(self.tags, index, HSPTag_TOMBSTONE);

    T *const value = (T *)&bucket.value;
    value->~T();

    cleanup(self, &bucket);
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
template <typename T, typename H, typename Eq, typename F>
void
for_each(HashSetProbing<T, H, Eq> &self, F f) noexcept {
  if (self.table) {
    for (std::size_t i = 0; i < capacity(self); ++i) {
      auto &bucket = self.table[i];
      if (bucket.tag == HSPTag_PRESENT) {
        T *const current = (T *)&bucket.value;
        f(*current);
      }
    }
  }
}

//=====================================
template <typename T, typename H, typename Eq>
void
swap(HashSetProbing<T, H, Eq> &f, HashSetProbing<T, H, Eq> &s) noexcept {
  using std::swap;
  swap(f.table, s.table);
  swap(f.tags, s.tags);
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
