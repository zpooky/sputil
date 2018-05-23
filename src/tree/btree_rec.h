#ifndef SP_UTIL_TREE_BTREE_REC_H
#define SP_UTIL_TREE_BTREE_REC_H

#include <collection/Array.h>
#include <cstddef>
#include <tuple>
#include <util/comparator.h>

namespace sp {
namespace rec {
//=====================================
template <typename T, std::size_t keys, typename Comparator>
struct BTNode {
  static constexpr std::size_t order = keys + 1;

  sp::UinStaticArray<T, keys> elements;
  sp::UinStaticArray<BTNode<T, keys, Comparator> *, order> children;

  BTNode() noexcept;
  ~BTNode() noexcept;

  BTNode(const BTNode<T, keys, Comparator> &) = delete;
  BTNode(const BTNode<T, keys, Comparator> &&) = delete;

  BTNode<T, keys, Comparator> &
  operator=(const BTNode<T, keys, Comparator> &) = delete;
  BTNode<T, keys, Comparator> &
  operator=(const BTNode<T, keys, Comparator> &&) = delete;
};

//=====================================
template <typename T, std::size_t keys, typename Comparator = sp::greater>
struct BTree {
  static_assert(keys > 1, "");

  BTNode<T, keys, Comparator> *root;

  BTree() noexcept;
  ~BTree() noexcept;

  BTree(const BTree<T, keys, Comparator> &) = delete;
  BTree(const BTree<T, keys, Comparator> &&) = delete;

  BTree &
  operator=(const BTree<T, keys, Comparator> &) = delete;
  BTree &
  operator=(const BTree<T, keys, Comparator> &&) = delete;
};

//=====================================
template <typename T, std::size_t keys, typename Comparator, typename Key>
T *
insert(BTree<T, keys, Comparator> &, Key &&) noexcept;

//=====================================
template <typename T, std::size_t keys, typename Comparator, typename Key>
const T *
find(const BTree<T, keys, Comparator> &, const Key &) noexcept;

template <typename T, std::size_t keys, typename Comparator, typename Key>
T *
find(BTree<T, keys, Comparator> &, const Key &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, std::size_t keys, typename Comparator>
BTNode<T, keys, Comparator>::BTNode() noexcept
    : elements{}
    , children{} {
}

template <typename T, std::size_t keys, typename Comparator>
BTNode<T, keys, Comparator>::~BTNode() noexcept {
  for (std::size_t i = 0; i < length(children); ++i) {
    delete children[i];
  }
}

template <typename T, std::size_t keys, typename Comparator>
BTree<T, keys, Comparator>::BTree() noexcept
    : root{nullptr} {
}

template <typename T, std::size_t keys, typename Comparator>
BTree<T, keys, Comparator>::~BTree() noexcept {
  if (root) {
    delete root;
    root = nullptr;
  }
}

//=====================================
namespace impl {
template <typename T, std::size_t keys, typename Cmp>
static bool
is_full(const BTNode<T, keys, Cmp> &node) noexcept {
  auto elems = length(node.elements);
  auto children = length(node.children);

  if (is_full(node.elements)) {
    assertxs(is_full(node.children), elems, children);
    return true;
  }

  assertxs(elems + 1 == children, elems, children);
  return false;
}

template <typename T, std::size_t keys, typename Cmp, typename Key>
static T *
bin_insert(BTNode<T, keys, Cmp> &dest, Key &&value,
           BTNode<T, keys, Cmp> *greater) noexcept {
  auto &children = dest.children;
  auto &elements = dest.elements;
  if (is_empty(children)) {
    assertxs(is_empty(elements), length(elements));
    BTNode<T, keys, Cmp> *less = nullptr;
    auto res = insert(children, less);
    assertx(res);
    assertx(*res == less);
  }

  T *const result = bin_insert(elements, std::forward<Key>(value));
  assertx(result);
  const std::size_t index = index_of(elements, result);
  assertxs(index != capacity(elements), index, length(elements),
           capacity(elements));

  auto gt = insert_at(children, index + 1, greater);
  assertx(gt);
  assertx(*gt == greater);
  return result;
}

template <typename T, std::size_t keys, typename Cmp>
static T *
median(const sp::UinStaticArray<T, keys> &elements, const T *extra) noexcept {
  std::size_t mid = (length(elements) + 1) / 2;
  assertxs(mid < length(elements), mid);
  if (elements[mid] > *extra) {
    //...
  } else {
    //...
  }
  // TODO
  return nullptr;
}

template <typename T, std::size_t keys, typename Cmp>
static void
partition(BTNode<T, keys, Cmp> &from, const T *pivot,
          BTNode<T, keys, Cmp> &to) noexcept {
  /* < pivot = left
   * > pivot = right
   * pivot should not be present in either... but it is for now...
   */
  auto &source_child = from.children;
  auto &source = from.elements;
  std::size_t drop = 0;
  const std::size_t idx = index_of(source, pivot);
  if (idx != capacity(source)) {
    assertx(&source[idx] == pivot);
    for (std::size_t i = idx + 1; i < length(source); ++i) {
      T *res = bin_insert(to, std::move(source[i]), source_child[i + 1]);
      assertx(res);
      // XXX: assert $res is the last in $to
      ++drop;
    }
  } else {
    // XXX: bin_last_lt(*pivot) and use the same ^ logic
    Cmp cmp;
    bool move = false;
    for (std::size_t i = 0; i < length(source); ++i) {
      if (cmp(source[i], *pivot)) {
        T *res = bin_insert(to, std::move(source[i]), source_child[i + 1]);
        assertx(res);
        // XXX: assert $res is the last in $to
        move = true;
        ++drop;
      } else {
        assertx(!move);
      }
    } // for
  }

  // drop last X moved elements & children
  drop_back(from.elements, drop);
  drop_back(from.children, drop);
}

template <typename T, std::size_t keys, typename Cmp>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
empty() noexcept {
  T *p = nullptr;
  BTNode<T, keys, Cmp> *r = nullptr;
  return std::make_tuple(p, r);
}

template <typename T, std::size_t keys, typename Cmp, typename Key>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
insert(BTNode<T, keys, Cmp> *tree, Key &&needle, T *&out) noexcept {
  if (tree == nullptr) {
    auto bubble = new T(std::forward<Key>(needle));
    BTNode<T, keys, Cmp> *greater = nullptr;

    return std::make_tuple(bubble, greater);
  }

  auto &children = tree->children;
  auto &elements = tree->elements;

  auto result = empty<T, keys, Cmp>();
  T *const gte = bin_find_gte<T, keys, Key, Cmp>(elements, needle);
  if (gte) {
    Cmp cmp;
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */
      out = gte;
      return empty<T, keys, Cmp>();
    }

    std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, capacity(elements));

    const auto child = children[index];
    result = insert(child, std::forward<Key>(needle), out);
  } else {
    assertxs(!is_empty(elements), length(elements));
    assertxs(!is_empty(children), length(children));

    auto child = last(children);
    assertxs(child, length(children));
    result = insert(*child, std::forward<Key>(needle), out);
  }

  T *const bubble = std::get<0>(result);
  if (bubble) {
    if (!is_full(*tree)) {
      BTNode<T, keys, Cmp> *greater = std::get<1>(result);
      T *res = bin_insert(*tree, std::move(*bubble), greater);
      assertx(res);

      delete bubble;
      return empty<T, keys, Cmp>();
    }
    const T *const m = median<T, keys, Cmp>(elements, bubble);
    assertx(m);

    auto right = new BTNode<T, keys, Cmp>;
    assertx(right); // TODO
    partition(*tree, m, *right);
    if (m != bubble) {
      // TODO assert $m is last in $tree
      // TODO insert $bubble in either $tree or $right
      // TODO move m from $tree -> $bubble
    }

    return std::make_tuple(bubble, right);
  }

  return empty<T, keys, Cmp>();
}
}
// TODO what is the log(order,elements) forumla to get the height?

template <typename T, std::size_t keys, typename Comparator, typename Key>
T *
insert(BTree<T, keys, Comparator> &self, Key &&value) noexcept {
  T *out = nullptr;
  auto result = impl::insert(self.root, std::forward<Key>(value), out);
  T *const bubble = std::get<0>(result);
  if (bubble) {
    if (self.root) {
      // TODO ...
    } else {
      self.root = new BTNode<T, keys, Comparator>;
      if (self.root) {
        BTNode<T, keys, Comparator> *greater = std::get<1>(result);
        T *res = impl::bin_insert(*self.root, std::move(*bubble), greater);
        assertx(res);
      }
    }
    delete bubble;
  }

  return out;
}

//=====================================
namespace impl {
template <typename T, std::size_t keys, typename Cmp, typename Key>
static const T *
find(const BTNode<T, keys, Cmp> *const tree, const Key &needle) noexcept {
  if (tree == nullptr) {
    return nullptr;
  }

  const auto &children = tree->children;
  const auto &elements = tree->elements;

  const T *const gte = bin_find_gte<T, keys, Key, Cmp>(elements, needle);
  if (gte) {
    Cmp cmp;
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */
      return gte;
    }

    /* Go down less than element child */
    std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, capacity(elements));

    const auto child = children[index];
    return find(child, needle);
  }

  /* needle is greater than any other element in elements */
  const auto child = last(children);
  assertxs(child, length(children));
  return find(*child, needle);
}
}

template <typename T, std::size_t keys, typename Comparator, typename Key>
const T *
find(const BTree<T, keys, Comparator> &self, const Key &needle) noexcept {
  return impl::find<T, keys, Comparator, Key>(self.root, needle);
}

template <typename T, std::size_t keys, typename Comparator, typename Key>
T *
find(BTree<T, keys, Comparator> &self, const Key &needle) noexcept {
  const auto &c_self = self;
  return (T *)find(c_self, needle);
}

//=====================================
}
}

#endif
