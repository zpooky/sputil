#ifndef SP_UTIL_MAP_HASH_SET_PROBING_H
#define SP_UTIL_MAP_HASH_SET_PROBING_H

#include <algorithm>
#include <cstddef>
#include <hash/standard.h>
#include <type_traits>
#include <util/QuadSet.h>
#include <util/array.h>
#include <util/assert.h>

#define HSPTag_EMPTY char(0)
#define HSPTag_PRESENT char(1)
#define HSPTag_TOMBSTONE char(2)

/*
 * Linear probing
 */
namespace sp {
//=====================================
template <typename T, typename Hash = sp::Hasher<T>,
          typename Eq = sp::Equality<T>>
struct HashSetProbing {
  using Bucket = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  using value_type = T;

  Bucket *table;
  sp::Quadset tags;

  std::size_t length;
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

//=====================================
template <typename T, typename H, typename Eq>
const T *
lookup_default(const HashSetProbing<T, H, Eq> &, const T &needle,
               const T &def) noexcept;

template <typename T, typename H, typename Eq>
T *
lookup_default(HashSetProbing<T, H, Eq> &, const T &needle, T &def) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
lookup_insert(HashSetProbing<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V, typename Compute>
T *
lookup_compute(HashSetProbing<T, H, Eq> &, const V &needle, Compute) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
contains(const HashSetProbing<T, H, Eq> &, const V &) noexcept;

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

template <typename T, typename H, typename Eq, typename F>
void
for_each(const HashSetProbing<T, H, Eq> &, F) noexcept;

//=====================================
namespace rec {
template <typename T, typename H, typename Eq>
bool
verify(const HashSetProbing<T, H, Eq> &) noexcept;
}

//=====================================
namespace n {
template <typename T, typename H, typename Eq>
std::size_t
length(const HashSetProbing<T, H, Eq> &) noexcept;
}

template <typename T, typename H, typename Eq>
std::size_t
length(const HashSetProbing<T, H, Eq> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::HashSetProbing(std::size_t cap) noexcept
    : table{nullptr}
    , tags{nullptr, 0} // TODO
    , length{0}
    , capacity{cap < 16 ? 16 : cap} {
}

template <typename T, typename H, typename Eq>
HashSetProbing<T, H, Eq>::~HashSetProbing() noexcept {
  if (this->table) {
    if (!std::is_trivially_destructible<T>::value) {
      std::size_t gced = 0;
      for (std::size_t idx = 0; idx < capacity && gced < length; ++idx) {
        const auto tag = test(this->tags, idx);

        if (tag == HSPTag_PRESENT) {
          auto &current = this->table[idx];
          ++gced;
          T *const v = (T *)&current;
          v->~T();
        }
      } // for
    }

    delete[] this->table;
  }

  this->table = nullptr;
  this->length = 0;
  this->capacity = 0;
}

//=====================================
namespace impl {
static inline std::size_t
index_of(std::size_t hash, std::size_t length) noexcept {
  const std::size_t res = hash & (length - 1);
  assertx_f({
    const std::size_t cmp = hash % length;
    assertxs(res < length, res, length);
    assertxs(cmp == res, hash, length, cmp, res);
  });
  return res;
}

template <typename T, typename H, typename Eq>
static T *
rehash(HashSetProbing<T, H, Eq> &self, const T *const needle) noexcept {
  // printf("rehash()\n");
  T *result = nullptr;

  HashSetProbing<T, H, Eq> tmp;
  tmp.capacity = self.capacity * 2;

  // XXX new and check non null before move

  std::size_t cnt = 0;
  for (std::size_t idx = 0; idx < capacity(self) && cnt < length(self); ++idx) {
    const auto tag = sp::test(self.tags, idx);

    if (tag == HSPTag_PRESENT) {
      auto &bucket = self.table[idx];

      ++cnt;
      sp::set(self.tags, idx, HSPTag_EMPTY);
      T *const current = (T *)&bucket;

      T *const ins = ::sp::insert(tmp, std::move(*current));
      if (current == needle) {
        result = ins;
      }
      assertx_n(ins);

      current->~T();
    }
  }

  assertxs(cnt == length(self), cnt, length(self));
  self.length = 0;

  assertx_f({
    rec::verify(self);
    rec::verify(tmp);
  });

  swap(self, tmp);
  assertx(result);
  return result;
}

template <typename T, typename H, typename Eq>
static bool
eager_resize(const HashSetProbing<T, H, Eq> &self) noexcept {
  constexpr std::size_t percent = 2;
  const std::size_t one_percent = self.capacity / 100;
  const std::size_t resz = std::max(one_percent * percent, std::size_t(1));
  const std::size_t resize_length = self.capacity - resz;

  bool res = self.length > resize_length;
  if (res) {
    // printf("cap[%zu],resize[%zu]\n", self.capacity, resize_length);
  }

  return res;
}

template <typename T, typename H, typename Eq, typename V, typename D,
          typename F>
T *
do_insert(HashSetProbing<T, H, Eq> &self, const V &needle, D dup,
          F fac) noexcept {
  if (!self.table) {
    using Bucket = typename HashSetProbing<T, H, Eq>::Bucket;
    // XXX check null
    assertxs(self.capacity > 0, self.capacity);
    self.table = new Bucket[self.capacity];

    self.tags.capacity = Quadset_number_of_buffer(self.capacity);
    assertxs(self.tags.capacity > 0, self.tags.capacity, self.capacity);
    self.tags.buffer = new std::uint64_t[self.tags.capacity]{0};

    assertx_f({
      for (std::size_t i = 0; i < self.tags.capacity; ++i) {
        assertxs(self.tags.buffer[i] == std::uint64_t(0), self.tags.buffer[i]);
      }
    });
  }

  if (self.table) {
    H hash;
    const std::size_t h = hash(needle);
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
        T *const b = (T *)&bucket;
        const T *const res = b;

        Eq equality;
        if (equality(*res, needle)) {

          /* Duplicate */
          return dup(*b, needle);
        }
      }

      idx = (idx + 1) % self.capacity;
    } while (idx != dest);

    if (empty != self.capacity) {
      assertxs(empty < self.capacity, empty, self.capacity);

      sp::set(self.tags, empty, HSPTag_PRESENT);
      T *const result = fac(empty);
      ++self.length;

      if (eager_resize(self)) {
        return rehash(self, result);
      }

      return result;
    }
  }

  return nullptr;
}
} // namespace impl

template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSetProbing<T, H, Eq> &self, V &&value) noexcept {
  auto on_dup = [](const auto &, const auto &) -> T * {
    /**/
    return nullptr;
  };

  auto on_factory = [&self, &value](std::size_t empty) -> T * {
    T *const result = (T *)(self.table + empty);
    return new (result) T(std::forward<V>(value));
  };

  const V &needle = value;
  return impl::do_insert(self, needle, on_dup, on_factory);
  // return nullptr;
}

//=====================================
namespace impl {
template <typename T, typename H, typename Eq, typename V>
T *
lookup_insert(HashSetProbing<T, H, Eq> &self, V &&needle,
              bool &inserted) noexcept {
  assertx(false);
  // TODO
  return nullptr;
}
}

template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSetProbing<T, H, Eq> &self, V &&needle) noexcept {
  using namespace impl;

  bool inserted = false;
  T *const result = lookup_insert(self, std::forward<V>(needle), inserted);

  if (result) {
    if (!inserted) {

      result->~T();
      new (result) T(std::forward<V>(needle));
    }
  }

  return result;
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
      const T *const res = (T *)&bucket;

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
} // namespace impl

template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;
  // printf("lookup()\n");

  const std::size_t index = lookup_bucket(self, needle);
  if (index != self.capacity) {
    const auto &bucket = self.table[index];
    return (const T *)&bucket;
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
template <typename T, typename H, typename Eq>
const T *
lookup_default(const HashSetProbing<T, H, Eq> &self, const T &needle,
               const T &def) noexcept {
  const T *result = lookup(self, needle, def);
  if (!result) {
    result = &def;
  }

  return result;
}

template <typename T, typename H, typename Eq>
T *
lookup_default(HashSetProbing<T, H, Eq> &self, const T &needle,
               T &def) noexcept {
  const auto &c_self = self;
  const auto &c_def = def;
  return (T *)lookup_default(c_self, needle, c_def);
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
lookup_insert(HashSetProbing<T, H, Eq> &self, V &&needle) noexcept {
  using namespace impl;

  bool inserted = false;
  return lookup_insert(self, std::forward<V>(needle), inserted);
}

//=====================================
template <typename T, typename H, typename Eq, typename V, typename C>
T *
lookup_compute(HashSetProbing<T, H, Eq> &self, const V &needle,
               C compute) noexcept {
  auto on_dup = [](T &bucket, const V &) -> T * {
    /**/
    return &bucket;
  };

  auto on_factory = [&self, &compute, &needle](std::size_t empty) -> T * {
    T *const result = (T *)(self.table + empty);
    compute(*result, needle);

    return result;
  };

  return impl::do_insert(self, needle, on_dup, on_factory);
}

//=====================================
namespace impl {
template <typename T, typename H, typename Eq>
static void
cleanup(HashSetProbing<T, H, Eq> &self,
        typename HashSetProbing<T, H, Eq>::Bucket *needle) noexcept {
  assertx(needle);

  const std::size_t capacity = self.capacity;
  const std::size_t dest = sp::index_of(self.table, capacity, capacity, needle);

  assertxs(dest != capacity, dest, capacity);

  const std::size_t next_idx = (dest + 1) % capacity;
  auto tag = sp::test(self.tags, next_idx);
  if (tag != HSPTag_EMPTY) {
    return;
  }

  std::size_t idx = dest;
  do {
    tag = sp::test(self.tags, idx);
    if (tag != HSPTag_TOMBSTONE) {
      // if (tag == HSPTag_PRESENT) {
      return;
    }

    assertx(tag == HSPTag_EMPTY || tag == HSPTag_TOMBSTONE);
    sp::set(self.tags, idx, HSPTag_EMPTY);

    idx = (idx - 1) % capacity;
  } while (idx != dest);
}
} // namespace impl

template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSetProbing<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;
  // printf("remve()\n");

  const std::size_t index = lookup_bucket(self, needle);
  if (index != self.capacity) {
    auto &bucket = self.table[index];
    sp::set(self.tags, index, HSPTag_TOMBSTONE);

    T *const value = (T *)&bucket;
    value->~T();

    --self.length;

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
      auto tag = sp::test(self.tags, i);

      if (tag == HSPTag_PRESENT) {
        T *const current = (T *)&bucket;
        f(*current);
      }
    }
  }
}

template <typename T, typename H, typename Eq, typename F>
void
for_each(const HashSetProbing<T, H, Eq> &self, F f) noexcept {
  if (self.table) {
    for (std::size_t i = 0; i < capacity(self); ++i) {
      auto &bucket = self.table[i];
      auto tag = sp::test(self.tags, i);

      if (tag == HSPTag_PRESENT) {
        const T *const current = (T *)&bucket;
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
  swap(f.length, s.length);
  swap(f.capacity, s.capacity);
}

//=====================================
namespace rec {
template <typename T, typename H, typename Eq>
bool
verify(const HashSetProbing<T, H, Eq> &self) noexcept {
  if (sp::n::length(self) != length(self)) {
    assertxs(sp::n::length(self) == length(self), sp::n::length(self),
             length(self), capacity(self));
    return false;
  }

  return true;
}
} // namespace rec

//=====================================
namespace n {
template <typename T, typename H, typename Eq>
std::size_t
length(const HashSetProbing<T, H, Eq> &self) noexcept {
  std::size_t result = 0;
  for_each(self, [&result](const auto &) {
    /**/
    ++result;
  });
  return result;
}
}

template <typename T, typename H, typename Eq>
std::size_t
length(const HashSetProbing<T, H, Eq> &self) noexcept {
  return self.length;
}

//=====================================
} // namespace sp

#endif
