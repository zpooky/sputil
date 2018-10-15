#ifndef SP_UTIL_MAP_HASH_MAP_PROBING_H
#define SP_UTIL_MAP_HASH_MAP_PROBING_H

#include <map/HashSetProbing.h>
#include <map/impl/entry.h>

// TODO make Tree & Probing have the same implementation
namespace sp {
//=====================================
template <typename Key, typename Value, typename H = sp::Hasher<Key>,
          typename Eq = sp::Equality<Key>>
struct HashMapProbing {
  using key_type = Key;
  using value_type = Value;

  using Entry = impl::HashMapEntry<Key, Value>;
  using Hash = impl::Hasher_HashMapEntry<Key, Value, H>;
  using Equality = impl::Equality_HashMapEntry<Key, Value, Eq>;

  HashSetProbing<Entry, Hash, Equality> set;

  HashMapProbing() noexcept;
  HashMapProbing(const HashMapProbing &) = delete;
  HashMapProbing(const HashMapProbing &&) = delete;

  ~HashMapProbing() noexcept;
};

//=====================================
template <typename K, typename V, typename H, typename Eq, typename Key,
          typename Value>
V *
insert(HashMapProbing<K, V, H, Eq> &, Key &&, Value &&) noexcept;

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V *
lookup(const HashMapProbing<Key, V, H, Eq> &, const K &) noexcept;

template <typename Key, typename V, typename H, typename Eq, typename K>
V *
lookup(HashMapProbing<Key, V, H, Eq> &, const K &) noexcept;

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V &
lookup_default(const HashMapProbing<Key, V, H, Eq> &, const K &,
               const V &) noexcept;

template <typename Key, typename V, typename H, typename Eq, typename K>
V &
lookup_default(HashMapProbing<Key, V, H, Eq> &, const K &, V &) noexcept;

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
bool
remove(HashMapProbing<Key, V, H, Eq> &, const K &) noexcept;

//=====================================
template <typename Key, typename V, typename H, typename Eq>
std::size_t
length(const HashMapProbing<Key, V, H, Eq> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename Key, typename Value, typename H, typename Eq>
HashMapProbing<Key, Value, H, Eq>::HashMapProbing() noexcept
    : set{} {
}

template <typename Key, typename Value, typename H, typename Eq>
HashMapProbing<Key, Value, H, Eq>::~HashMapProbing() noexcept {
}

//=====================================
template <typename K, typename V, typename H, typename Eq, typename Key,
          typename Value>
V *
insert(HashMapProbing<K, V, H, Eq> &self, Key &&key, Value &&value) noexcept {
  using Entry = impl::HashMapEntry<K, V>;

  bool inserted = false;
  auto on_compute = [key = std::forward<decltype(key)>(key),
                     value = std::forward<decltype(value)>(value),
                     &inserted](auto &bucket, const auto &) {
    inserted = true;

    return new (&bucket) Entry(std::forward<decltype(key)>(key),
                               std::forward<decltype(value)>(value));
  };

  const Key &needle = key;
  Entry *const res = lookup_compute(self.set, needle, on_compute);
  if (res) {
    V *const result = &res->value;

    if (!inserted) {
      result->~V();
      new (result) V{std::forward<Value>(value)};
    }

    return result;
  }

  return nullptr;
}

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V *
lookup(const HashMapProbing<Key, V, H, Eq> &self, const K &needle) noexcept {
  using Entry = impl::HashMapEntry<Key, V>;

  const Entry *const result = lookup(self.set, needle);
  if (result) {
    return &result->value;
  }

  return nullptr;
}

template <typename Key, typename V, typename H, typename Eq, typename K>
V *
lookup(HashMapProbing<Key, V, H, Eq> &self, const K &needle) noexcept {
  const auto &c_self = self;
  return (V *)lookup(c_self, needle);
}

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V &
lookup_default(const HashMapProbing<Key, V, H, Eq> &self, const K &needle,
               const V &def) noexcept {
  using Entry = impl::HashMapEntry<Key, V>;

  const Entry *const result = lookup(self.set, needle);
  if (result) {
    return result->value;
  }

  return def;
}

template <typename Key, typename V, typename H, typename Eq, typename K>
V &
lookup_default(HashMapProbing<Key, V, H, Eq> &self, const K &needle,
               V &def) noexcept {
  using Entry = impl::HashMapEntry<Key, V>;

  Entry *const result = lookup(self.set, needle);
  if (result) {
    return result->value;
  }

  return def;
}

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
bool
remove(HashMapProbing<Key, V, H, Eq> &self, const K &needle) noexcept {
  return remove(self.set, needle);
}

//=====================================
template <typename Key, typename V, typename H, typename Eq>
std::size_t
length(const HashMapProbing<Key, V, H, Eq> &self) noexcept {
  return length(self.set);
}

//=====================================
} // namespace sp

#endif
