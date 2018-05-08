#ifndef SP_UTIL_MAP_SET_H
#define SP_UTIL_MAP_SET_H

#include <hash/util.h>
#include <tree/bst.h>

namespace sp {
namespace impl {
template <typename T>
struct HSBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  st value;
  HSBucket *next;

  HSBucket() noexcept;
  ~HSBucket() noexcept;
};

template <typename T>
struct HSNode {
  std::size_t start;
  std::size_t stop;
  HSBucket<T> buckets[256];

  HSNode(std::size_t strt, std::size_t stp) noexcept;

  bool operator>(std::size_t) const noexcept;
};

} // namespace sp::impl

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
template <typename T, sp::Hasher<T> h, typename V>
bool
contains(const HashSet<T, h> &, const V &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
HSBucket<T>::HSBucket() noexcept
    : next{nullptr} {
}

template <typename T>
HSBucket<T>::~HSBucket() noexcept {
}

//=====================================
template <typename T>
HSNode<T>::HSNode(std::size_t strt, std::size_t stp) noexcept
    : start(strt)
    , stop{stp}
    , buckets() {
}

template <typename T>
bool
HSNode<T>::operator>(std::size_t) const noexcept {
  // TODO
  return false;
}

} // namespace sp::impl
//=====================================
template <typename T, sp::Hasher<T> h>
HashSet<T, h>::HashSet() noexcept
    : entries(0) // TODO move to node level
    , tree() {
}

//=====================================
template <typename T, sp::Hasher<T> hash, typename V>
T *
insert(HashSet<T, hash> &self, V &&val) noexcept {
  using namespace impl;
  auto &tree = self.tree;
  const std::size_t code = hash(val);
  // HSNode<T> *node = find(tree, code);
  // if (node) {
  //   // TODO
  // }
  return nullptr;
}

//=====================================
}

#endif
