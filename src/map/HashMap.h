#ifndef SP_UTIL_MAP_HASH_MAP_H
#define SP_UTIL_MAP_HASH_MAP_H

#include <map/HashSet.h>

namespace sp {
namespace impl {

template <typename Key, typename Value>
struct HashMapEntry {
  Key key;
  Value value;

  template <typename K, typename V>
  HashMapEntry(K &&k, V &&v) noexcept
      : key(std::forward<K>(k))
      , value(std::forward<V>(v)) {
  }

  bool
  operator==(const HashMapEntry &o) const noexcept {
    return key == o.key;
  }

  bool
  operator==(const Key &o) const noexcept {
    return key == o;
  }

  ~HashMapEntry() noexcept {
  }
};

template <typename Key, typename Value, sp::hasher<Key> impl>
static std::size_t
hasher_HashMap(const HashMapEntry<Key, Value> &in) noexcept {
  return impl(in.key);
}
}

template <typename Key, typename Value, sp::hasher<Key> h>
struct HashMap {
  using Entry = impl::HashMapEntry<Key, Value>;

  HashSet<Entry, impl::hasher_HashMap<Key, Value, h>> set;

  HashMap() noexcept;
  HashMap(const HashMap &) = delete;
  HashMap(const HashMap &&) = delete;

  ~HashMap() noexcept;
};

//=====================================
template <typename K, typename V, sp::hasher<K> h, typename Key, typename Value>
V *
insert(HashMap<K, V, h> &, Key &&, Value &&) noexcept;

//=====================================
template <typename Key, typename V, sp::hasher<Key> h>
const V *
lookup(const HashMap<Key, V, h> &, const Key &) noexcept;

template <typename Key, typename V, sp::hasher<Key> h>
V *
lookup(HashMap<Key, V, h> &, const Key &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename Key, typename Value, sp::hasher<Key> h>
HashMap<Key, Value, h>::HashMap() noexcept
    : set{} {
}

template <typename Key, typename Value, sp::hasher<Key> h>
HashMap<Key, Value, h>::~HashMap() noexcept {
}

//=====================================
template <typename K, typename V, sp::hasher<K> h, typename Key, typename Value>
V *
insert(HashMap<K, V, h> &self, Key &&key, Value &&value) noexcept {
  auto res = upsert(self.set, impl::HashMapEntry<K, V>(key, value));
  if (res) {
    return &res->value;
  }

  return nullptr;
}

//=====================================
template <typename Key, typename V, sp::hasher<Key> hash>
const V *
lookup(const HashMap<Key, V, hash> &self, const Key &needle) noexcept {
  const impl::HashKey code(hash(needle));
  auto *res = sp::impl::lookup(self.set, code, needle);
  if (res) {
    return &res->value;
  }

  return nullptr;
}

template <typename Key, typename V, sp::hasher<Key> h>
V *
lookup(HashMap<Key, V, h> &self, const Key &needle) noexcept {
  const auto &c_self = self;
  return (V *)lookup(c_self, needle);
}

//=====================================
}

#endif
