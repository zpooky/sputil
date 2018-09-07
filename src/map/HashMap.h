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

template <typename Key, typename Value, typename H>
struct hasher_HashMap {
  std::size_t
  operator()(const HashMapEntry<Key, Value> &in) noexcept {
    H hash;
    return hash(in.key);
  }
};
}

template <typename Key, typename Value, typename H>
struct HashMap {
  using Entry = impl::HashMapEntry<Key, Value>;

  HashSet<Entry, impl::hasher_HashMap<Key, Value, H>> set;

  HashMap() noexcept;
  HashMap(const HashMap &) = delete;
  HashMap(const HashMap &&) = delete;

  ~HashMap() noexcept;
};

//=====================================
template <typename K, typename V, typename H, typename Key, typename Value>
V *
insert(HashMap<K, V, H> &, Key &&, Value &&) noexcept;

//=====================================
template <typename Key, typename V, typename H>
const V *
lookup(const HashMap<Key, V, H> &, const Key &) noexcept;

template <typename Key, typename V, typename H>
V *
lookup(HashMap<Key, V, H> &, const Key &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename Key, typename Value, typename H>
HashMap<Key, Value, H>::HashMap() noexcept
    : set{} {
}

template <typename Key, typename Value, typename H>
HashMap<Key, Value, H>::~HashMap() noexcept {
}

//=====================================
template <typename K, typename V, typename H, typename Key, typename Value>
V *
insert(HashMap<K, V, H> &self, Key &&key, Value &&value) noexcept {
  auto res = upsert(self.set, impl::HashMapEntry<K, V>(key, value));
  if (res) {
    return &res->value;
  }

  return nullptr;
}

//=====================================
template <typename Key, typename V, typename H>
const V *
lookup(const HashMap<Key, V, H> &self, const Key &needle) noexcept {
  H hash;
  const impl::HashKey code(hash(needle));
  auto *res = sp::impl::lookup(self.set, code, needle);
  if (res) {
    return &res->value;
  }

  return nullptr;
}

template <typename Key, typename V, typename H>
V *
lookup(HashMap<Key, V, H> &self, const Key &needle) noexcept {
  const auto &c_self = self;
  return (V *)lookup(c_self, needle);
}

//=====================================
}

#endif
