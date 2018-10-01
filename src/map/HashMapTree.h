#ifndef SP_UTIL_MAP_HASH_MAP_H
#define SP_UTIL_MAP_HASH_MAP_H

#include <map/HashSetTree.h>

namespace sp {
//=====================================
namespace impl {
template <typename Key, typename Value>
struct HashMapTreeEntry {
  Key key;
  Value value;

  template <typename K, typename V>
  HashMapTreeEntry(K &&k, V &&v) noexcept
      : key(std::forward<K>(k))
      , value(std::forward<V>(v)) {
  }

  HashMapTreeEntry(const HashMapTreeEntry &) = delete;
  HashMapTreeEntry(HashMapTreeEntry &&o) noexcept
      : key{std::move(o.key)}
      , value{std::move(o.value)} {
  }

  ~HashMapTreeEntry() noexcept {
  }
};

//=====================================
template <typename K, typename V, typename H>
struct Hasher_HashMapTree {
  using Entry = HashMapTreeEntry<K, V>;

  std::size_t
  operator()(const K &in) noexcept {
    H hash;
    return hash(in);
  }

  std::size_t
  operator()(const Entry &in) noexcept {
    return operator()(in.key);
  }

  template <typename N>
  std::size_t
  operator()(const N &in) noexcept {
    H hash;
    return hash(in);
  }
};

template <typename K, typename V, typename Eq>
struct Equality_HashMapTree {
  using Entry = HashMapTreeEntry<K, V>;

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

  template <typename N>
  bool
  operator()(const Entry &f, const N &s) const noexcept {
    Eq equality;
    return equality(f.key, s);
  }
};
} // namespace impl

//=====================================
template <typename Key, typename Value, typename H = sp::Hasher<Key>,
          typename Eq = sp::Equality<Key>>
struct HashMapTree {
  using key_type = Key;
  using value_type = Value;

  using Entry = impl::HashMapTreeEntry<Key, Value>;
  using Hash = impl::Hasher_HashMapTree<Key, Value, H>;
  using Equality = impl::Equality_HashMapTree<Key, Value, Eq>;

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
  using Entry = impl::HashMapTreeEntry<K, V>;

  bool inserted = false;
  auto compute = [key = std::forward<decltype(key)>(key),
                  value = std::forward<decltype(value)>(value),
                  &inserted](auto &bucket, const auto &) {
    inserted = true;

    return new (&bucket.value) Entry(std::forward<decltype(key)>(key),
                                     std::forward<decltype(value)>(value));
  };

  V *result = nullptr;
  Entry *const res = lookup_compute(self.set, key, compute);
  if (res) {
    result = &res->value;

    if (!inserted) {
      result->~V();
      new (result) V{std::forward<Value>(value)};
    }
  }

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
