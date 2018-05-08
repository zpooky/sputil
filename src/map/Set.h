#ifndef SP_UTIL_MAP_SET_H
#define SP_UTIL_MAP_SET_H

#include <hash/util.h>
#include <tree/bst.h>

namespace sp {
namespace impl {
template <typename T>
struct HSBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  HSBucket *next;
  st value;
  bool present;

  HSBucket() noexcept;
  ~HSBucket() noexcept;
};

struct HashKey;

template <typename T, std::size_t capacity = 256>
struct HSNode {
  const std::size_t start;
  std::size_t length;

  HSBucket<T> buckets[capacity];

  HSNode(std::size_t, std::size_t) noexcept;

  bool
  operator>(const HashKey &) const noexcept;

  bool
  operator>(const HSNode<T> &) const noexcept;
};

struct HashKey {
  const std::size_t hash;

  HashKey(std::size_t h) noexcept
      : hash(h) {
  }

  template <typename T>
  bool
  operator>(const HSNode<T> &o) const noexcept {
    printf("hash[%zu] > o[o.start[%zu+o.length[%zu]: %zu]\n", hash, o.start,
           o.length, o.start + o.length);
    return hash > (o.start + o.length);
  }
};

} // namespace impl

template <typename T, sp::Hasher<T>>
struct HashSet {
  std::size_t entries;
  binary::Tree<impl::HSNode<T>> tree;

  HashSet() noexcept;
};

//=====================================
template <typename T, sp::Hasher<T> h, typename V>
T *
insert(HashSet<T, h> &, V &&) noexcept;

//=====================================
// template <typename T, sp::Hasher<T> h, typename V>
// bool
// contains(const HashSet<T, h> &, const V &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
HSBucket<T>::HSBucket() noexcept
    : next{nullptr}
    , present(false) {
}

template <typename T>
HSBucket<T>::~HSBucket() noexcept {
}

//=====================================
template <typename T, std::size_t c>
HSNode<T, c>::HSNode(std::size_t strt, std::size_t len) noexcept
    : start(strt)
    , length{len}
    , buckets() {
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator>(const HashKey &o) const noexcept {
  //TODO fix here bug
  bool res = (start + length) > o.hash;
  printf("(start[%zu]+length[%zu]: %zu) > o.hash[%zu]: "
         "%s\n",
         start, length, start + length, //
         o.hash,                        //
         res ? "true" : "false");
  return res;
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator>(const HSNode<T> &o) const noexcept {
  bool res = (start + length) > (o.start + o.length);
  printf("(start[%zu]+length[%zu]: %zu) > (o.start[%zu]+o.length[%zu]: %zu): "
         "%s\n",
         start, length, start + length, o.start, o.length, o.start + o.length,
         res ? "true" : "false");
  return res;
}

} // namespace impl
//=====================================
template <typename T, sp::Hasher<T> h>
HashSet<T, h>::HashSet() noexcept
    : entries(0) // TODO move to node level
    , tree() {
}

//=====================================
namespace impl {

// #<{(|*
//  * Returns a power of two size for the given target capacity.
//  |)}>#
// static final int tableSizeFor(int cap) {
//     int n = cap - 1;
//     n |= n >>> 1;
//     n |= n >>> 2;
//     n |= n >>> 4;
//     n |= n >>> 8;
//     n |= n >>> 16;
//     return (n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1;
// }
template <typename T, typename V>
T *
insert(HSBucket<T> &node, V &&val) noexcept {
  HSBucket<T> *current = &node;
Lit:
  if (current) {
    if (current->present) {
      T *const value = (T *)&current->value;
      if (*value == val) {
        // already present
        return nullptr;
      }

      if (current->next) {
        current = current->next;
        goto Lit;
      }

      current = current->next = new (std::nothrow) HSBucket<T>;
    }

    if (current) {
      current->present = true;
      return new (&current->value) T(std::forward<V>(val));
    }
  }

  return nullptr;
}

static inline std::size_t
index_of(const HashKey &code, std::size_t length) noexcept {
  return code.hash & (length - 1);
}

template <typename T, std::size_t c, typename V>
T *
insert(HSNode<T, c> &node, const HashKey &code, V &&val) noexcept {
  std::size_t index = index_of(code, node.length);
  HSBucket<T> &bucket = node.buckets[index];
  return insert(bucket, std::forward<V>(val));
}
} // namespace impl

template <typename T, sp::Hasher<T> hash, typename V>
T *
insert(HashSet<T, hash> &self, V &&val) noexcept {
  using namespace impl;

  auto &tree = self.tree;
  HashKey code(hash(val));

  HSNode<T> *node = find(tree, code);
  if (node == nullptr) {
    printf(".\n");
    // should only get here on first invocation
    HSNode<T> tmp(0, 256);
    auto ires = insert(tree, tmp); // TODO length
    node = std::get<0>(ires);
    // assertxs(!node || std::get<1>(ires), node, std::get<1>(ires));
  }

  if (node) {
    return impl::insert(*node, code, std::forward<T>(val));
  }

  return nullptr;
}

//=====================================
} // namespace sp

#endif
