#ifndef SP_UTIL_MAP_IMPL_ENTRY_H
#define SP_UTIL_MAP_IMPL_ENTRY_H

#include <cstddef>
#include <algorithm>

namespace sp {
namespace impl {
//=====================================
template <typename Key, typename Value>
struct HashMapEntry {
  Key key;
  Value value;

  template <typename K, typename V>
  HashMapEntry(K &&k, V &&v) noexcept
      : key(std::forward<K>(k))
      , value(std::forward<V>(v)) {
  }

  HashMapEntry(const HashMapEntry &) = delete;
  HashMapEntry(HashMapEntry &&o) noexcept
      : key{std::move(o.key)}
      , value{std::move(o.value)} {
  }

  ~HashMapEntry() noexcept {
  }
};

//=====================================
template <typename Key, typename Value, typename H>
struct Hasher_HashMapEntry {
  using Entry = HashMapEntry<Key, Value>;

  std::size_t
  operator()(const Key &in) noexcept {
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

//=====================================
template <typename K, typename V, typename Eq>
struct Equality_HashMapEntry {
  using Entry = HashMapEntry<K, V>;

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

//=====================================
} // namespace impl
} // namespace sp

#endif
