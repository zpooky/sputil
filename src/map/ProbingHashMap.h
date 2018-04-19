#ifndef SP_UTIL_MAP_PROBING_HASH_MAP_H
#define SP_UTIL_MAP_PROBING_HASH_MAP_H

#include <collection/Array.h>
#include <cstddef>
#include <hash/util.h>
#include <util/maybe.h>

namespace sp {
template <typename Key, typename Value, typename H = Hasher<Key>>
struct ProbingHashMap {
  /**/
};

template <typename Value, std::size_t cap, typename H = Hasher<Value>>
struct StaticProbingHashSet {
  /**/
  Value **key[cap];
  Value *value[cap];

  StaticProbingHashSet() noexcept;
  StaticProbingHashSet(UinStaticArray<Value, cap> &) noexcept;
};

//=====================================
template <typename Value, std::size_t cap, typename H>
StaticProbingHashSet<Value, cap, H>
create(Value (&)[cap]) noexcept;

//=====================================
template <typename Value, std::size_t cap, typename H, typename K>
Value *
lookup(StaticProbingHashSet<Value, cap, H> &, const K &) noexcept;

//=====================================

/* ======================================================= */
/* ======================================================= */
/* ======================================================= */

template <typename Value, std::size_t cap, typename H>
StaticProbingHashSet<Value, cap, H>::StaticProbingHashSet() noexcept
    : key{nullptr}
    , value{nullptr} {
}

template <typename Value, std::size_t cap, typename H>
StaticProbingHashSet<Value, cap, H>::StaticProbingHashSet(
    UinStaticArray<Value, cap> &) noexcept
    : key{nullptr}
    , value{nullptr} {
}

//=====================================
// template <typename Value, std::size_t cap, typename H>
// StaticProbingHashSet<Value, cap, H>
// create(Value (&)[cap]) noexcept {
//
//   return {};
// }
//=====================================
template <typename Value, std::size_t cap, typename H, typename K>
Value *
lookup(StaticProbingHashSet<Value, cap, H> &self, const K &needle) noexcept {
  H h;
  assertx(h);
  std::size_t idx = h(needle);

  return nullptr;
}

} // namespace sp

#endif
