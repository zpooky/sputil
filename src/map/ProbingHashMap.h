#ifndef SP_UTIL_MAP_PROBING_HASH_MAP_H
#define SP_UTIL_MAP_PROBING_HASH_MAP_H

#include <util/assert.h>
#include <collection/Array.h>
#include <cstddef>
#include <hash/util.h>
#include <util/maybe.h>

namespace sp {
template <typename Key, typename Value, sp::hasher<Key>>
struct ProbingHashMap {
  /**/
};

template <typename Value, std::size_t cap, sp::hasher<Value>>
struct StaticProbingHashSet {
  using node_type = Value *;
  /**/
  node_type *key[cap];
  node_type value[cap];

  StaticProbingHashSet() noexcept;
  StaticProbingHashSet(UinStaticArray<Value, cap> &) noexcept;
};

//=====================================
// template <typename Value, std::size_t cap, sp::hasher<Value> h>
// StaticProbingHashSet<Value, cap, h>
// create(Value (&)[cap]) noexcept;

//=====================================
template <typename Value, std::size_t cap, sp::hasher<Value> h, typename K>
Value *
lookup(StaticProbingHashSet<Value, cap, h> &, const K &) noexcept;

//=====================================

/* ======================================================= */
/* ======================================================= */
/* ======================================================= */

template <typename Value, std::size_t cap, sp::hasher<Value> h>
StaticProbingHashSet<Value, cap, h>::StaticProbingHashSet() noexcept
    : key{nullptr}
    , value{nullptr} {
  // TODO
  //
}

template <typename Value, std::size_t cap, sp::hasher<Value> h>
StaticProbingHashSet<Value, cap, h>::StaticProbingHashSet(
    UinStaticArray<Value, cap> &in) noexcept
    : key{nullptr}
    , value{nullptr} {
  for_each(in, [](Value &current) {
    const std::size_t hash = h(current);
    const std::size_t idx = hash % cap;

  });
  // assertx(false);
}

//=====================================
// template <typename Value, std::size_t cap, sp::hasher<Value> h>
// StaticProbingHashSet<Value, cap, h>
// create(Value (&)[cap]) noexcept {
//
//   return {};
// }
//=====================================
template <typename Value, std::size_t cap, sp::hasher<Value> h, typename K>
Value *
lookup(StaticProbingHashSet<Value, cap, h> &self, const K &needle) noexcept {
  const std::size_t hash = h(needle);
  const std::size_t idx = hash % cap;
  auto key = self.key[idx];
  if (key) {
    Value *it = *key;
    // const Value *const end = self.value + cap;
    // while (it != end) {
    //   ++it;
    // }
  }

  return nullptr;
}

} // namespace sp

#endif
