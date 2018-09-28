#ifndef SP_UTIL_MAP_HASH_MAP_PROBING_H
#define SP_UTIL_MAP_HASH_MAP_PROBING_H

#include <map/HashSetProbing.h>

namespace sp {
//=====================================
namespace impl {
template <typename Key, typename Value>
struct HashMapProbingEntry {
  Key key;
  Value value;

  template <typename K, typename V>
  HashMapProbingEntry(K &&k, V &&v) noexcept
      : key(std::forward<K>(k))
      , value(std::forward<V>(v)) {
  }

  ~HashMapProbingEntry() noexcept {
  }
};

//=====================================
template <typename Key, typename Value, typename H>
struct Hasher_HashMapProbing {

  using Entry = HashMapProbingEntry<Key, Value>;

  std::size_t
  operator()(const Key &in) noexcept {
    H hash;
    return hash(in);
  }

  std::size_t
  operator()(const Entry &in) noexcept {
    return operator()(in.key);
  }
};

template <typename K, typename V, typename Eq>
struct Equality_HashMapProbing {
  using Entry = HashMapProbingEntry<K, V>;

  bool
  operator()(const K &f, const K &s) const noexcept {
    Eq equality;
    return equality(f, s);
  }

  bool
  operator()(const Entry &f, const K &s) const noexcept {
    return operator()(f.key, s);
  }

  bool
  operator()(const Entry &f, const Entry &s) const noexcept {
    return operator()(f.key, s.key);
  }
};
}

//=====================================
template <typename Key, typename Value, typename H = sp::Hasher<Key>,
          typename Eq = sp::Equality>
struct HashMapProbing {
  using Entry = impl::HashMapProbingEntry<Key, Value>;
  using Hash = impl::Hasher_HashMapProbing<Key, Value, H>;
  using Equality = impl::Equality_HashMapProbing<Key, Value, Eq>;

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
insert(HashMapProbing<K, V, H, Eq> &, Key &&, Value &&) noexcept {
  // TODO
  return nullptr;
}

//=====================================
template <typename Key, typename V, typename H, typename Eq, typename K>
const V *
lookup(const HashMapProbing<Key, V, H, Eq> &self, const K &needle) noexcept {
  using Entry = impl::HashMapProbingEntry<Key, V>;
  // TODO

  // Entry *const result = lookup(self.set, needle);
  // if (result) {
  //   return &result->value;
  // }

  return nullptr;
}

template <typename Key, typename V, typename H, typename Eq, typename K>
V *
lookup(HashMapProbing<Key, V, H, Eq> &self, const K &needle) noexcept {
  const auto &c_self = self;
  return (V *)lookup(c_self, needle);
}
}

#endif
