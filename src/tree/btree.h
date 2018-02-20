#ifndef SP_UTIL_TREE_BTREE_H
#define SP_UTIL_TREE_BTREE_H

#include <collection/Array.h>
#include <tuple>

namespace btree {
namespace impl {
namespace btree {

/*
 * rank = keys?
 *
 * t =  Minimum degree (defines the range for number of keys)
 * keys = [2*t-1];
 * children = [2*t]
 */
template <typename T, std::size_t keys>
struct BTNode {
  using value_type = T;

  BTNode<T, keys> *parent;
  sp::UinStaticArray<T, keys> elements;
  sp::UinStaticArray<BTNode<T, keys> *, keys + 1> children;

  BTNode() noexcept;
};
}
}

template <typename T, std::size_t keys, typename Comparator = sp::greater>
struct Tree {
  using value_type = T;
  using reference = value_type &;
  using const_reference = const reference;
  using pointer = value_type *;
  using const_pointer = const value_type *;

  btree::impl::btree::BTNode<T, keys> *root;

  Tree() noexcept;

  Tree(const Tree<T, keys, Comparator> &) = delete;
  Tree(const Tree<T, keys, Comparator> &&) = delete;

  Tree &
  operator=(const Tree<T, keys, Comparator> &) = delete;
  Tree &
  operator=(const Tree<T, keys, Comparator> &&) = delete;

  ~Tree() noexcept;
};

template <typename T, std::size_t k, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, k, C> &, K &&) noexcept;

template <typename T, std::size_t k, typename C, typename K>
const T *
find(const Tree<T, k, C> &, const K &) noexcept;

template <typename T, std::size_t k, typename C, typename K>
T *
find(Tree<T, k, C> &, const K &) noexcept;

/*
 * template <typename T, typename C, typename K>
 * bool
 * remove(Tree<T, C> &, const K &) noexcept;
 *
 * template <typename T, typename C>
 * void
 * dump(Tree<T, C> &tree, std::string prefix = "") noexcept;
 *
 * template <typename T, typename C>
 * bool
 * verify(Tree<T, C> &tree) noexcept;
 */

/*
 * ==========================================================================
 */
namespace impl {
namespace btree {
template <typename T, std::size_t keys>
BTNode<T, keys>::BTNode() noexcept
    : parent{nullptr}
    , elements()
    , children() {
}
}
}

template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::Tree() noexcept
    : root(nullptr) {
}

template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::~Tree() noexcept {
  // TODO
}

template <typename T, std::size_t k, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, k, C> &tree, K &&v) noexcept {
  using namespace btree::impl::btree;
  if (tree.root) {
    // TODO
  } else {
    auto node = tree.root = new BTNode<T, k>;
    if (node) {
      auto res = bin_insert(node->elements, std::forward<K>(v));
      if (res) {
        return std::make_tuple(res, true);
      }
    }
  }
  return std::make_tuple(nullptr, false);
}

template <typename T, std::size_t k, typename C, typename K>
const T *
find(const Tree<T, k, C> &tree, const K &needle) noexcept {
  const auto *node = tree.root;

Lit:
  if (node) {
    const auto &elems = node->elements;

    std::size_t idx = 0;
    for (; idx < elems.length; ++idx) {
      C cmp;
      const auto *current = sp::get(elems, idx);
      if /*current > needle*/ (cmp(*current, needle)) {
        break;
      } /*current < needle*/ else if (cmp(needle, *current)) {
      } else {
        /*
         * match !(needle <> current)
         */
        return current;
      }
    }
    /* Check that we are not in leaf node */
    const auto next = sp::get(node->children, idx);
    if (next) {
      node = *next;
    }
    goto Lit;
  }

  return nullptr;
}

template <typename T, std::size_t k, typename C, typename K>
T *
find(Tree<T, k, C> &tree, const K &needle) noexcept {
  const auto &c_tree = tree;
  return (T *)find(c_tree, needle);
}
}
#endif
