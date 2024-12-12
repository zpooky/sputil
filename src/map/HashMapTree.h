#ifndef SP_UTIL_MAP_HASH_MAP_H
#define SP_UTIL_MAP_HASH_MAP_H

#include <map/HashSetTree.h>
#include <map/impl/entry.h>
#include <utility>
#include <cstdint>

namespace sp {
//=====================================
template <typename Key, typename Value, typename H = sp::Hasher<Key>,
          typename Eq = sp::Equality<Key>>
struct HashMapTree {
  using key_type = Key;
  using value_type = Value;

  using Entry = impl::HashMapEntry<Key, Value>;
  using Hash = impl::Hasher_HashMapEntry<Key, Value, H>;
  using Equality = impl::Equality_HashMapEntry<Key, Value, Eq>;

  HashSetTree<Entry, Hash, Equality> set;

  HashMapTree() noexcept;
  HashMapTree(const HashMapTree &) = delete;
  HashMapTree(const HashMapTree &&) = delete;

  ~HashMapTree() noexcept;
};

//=====================================
template <typename K, typename V, typename H, typename Eq, typename Key,
          typename Value>
V *
insert(HashMapTree<K, V, H, Eq> &, Key &&, Value &&) noexcept;

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V *
lookup(const HashMapTree<Key, V, H, Eq> &, const K &) noexcept;

template <typename Key, typename V, typename H, typename Eq, typename K>
V *
lookup(HashMapTree<Key, V, H, Eq> &, const K &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename Key, typename Value, typename H, typename Eq>
HashMapTree<Key, Value, H, Eq>::HashMapTree() noexcept
    : set{} {
}

template <typename Key, typename Value, typename H, typename Eq>
HashMapTree<Key, Value, H, Eq>::~HashMapTree() noexcept {
}

//=====================================
template <typename K, typename V, typename H, typename Eq, typename Key,
          typename Value>
V *
insert(HashMapTree<K, V, H, Eq> &self, Key &&key, Value &&value) noexcept {
  using Entry = impl::HashMapEntry<K, V>;

  assertx(false);
  V *result = nullptr;
#if 0
  bool inserted = false;
  auto compute = [key = std::forward<decltype(key)>(key),
                  value = std::forward<decltype(value)>(value),
                  &inserted](auto &bucket, const auto &) {
    inserted = true;

    return new (&bucket) Entry(std::forward<decltype(key)>(key),
                               std::forward<decltype(value)>(value));
  };

  Entry *const res = lookup_compute(self.set, key, compute);
  if (res) {
    result = &res->value;

    if (!inserted) {
      result->~V();
      new (result) V{std::forward<Value>(value)};
    }
  }
#endif

  return result;
}

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V *
lookup(const HashMapTree<Key, V, H, Eq> &self, const K &needle) noexcept {
  auto *res = lookup(self.set, needle);
  if (res) {
    return &res->value;
  }

  return nullptr;
}

template <typename Key, typename V, typename H, typename Eq, typename K>
V *
lookup(HashMapTree<Key, V, H, Eq> &self, const K &needle) noexcept {
  const auto &c_self = self;
  return (V *)lookup(c_self, needle);
}

//=====================================
} // namespace sp

#endif
