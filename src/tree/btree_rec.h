#ifndef SP_UTIL_TREE_BTREE_REC_H
#define SP_UTIL_TREE_BTREE_REC_H

#include <collection/Array.h>
#include <cstddef>
#include <tuple>
#include <util/comparator.h>

namespace sp {
namespace rec {
//=====================================
template <typename T, std::size_t k, typename Comparator>
struct BTNode {
  static constexpr std::size_t keys = k;
  static constexpr std::size_t order = keys + 1;
  using value_type = T;

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
  static_assert(keys >= 2, "");
  using value_type = T;

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
template <typename T, std::size_t keys, typename Comparator, typename Key>
bool
remove(BTree<T, keys, Comparator> &, const Key &) noexcept;

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
    if (children[i]) {
      delete children[i];
    }
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

template <typename T, std::size_t keys, typename Cmp>
static bool
is_empty(const BTNode<T, keys, Cmp> &node) noexcept {
  auto elems = length(node.elements);
  auto children = length(node.children);

  if (is_empty(node.elements)) {
    assertxs(is_empty(node.children), elems, children);
    return true;
  }

  assertxs(elems + 1 == children, elems, children);
  return false;
}

//=====================================
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

  Cmp cmp;
  T *const result = bin_insert(elements, std::forward<Key>(value), cmp);
  assertxs(result, length(elements), capacity(elements), length(children),
           capacity(children));
  const std::size_t index = index_of(elements, result);
  assertxs(index != capacity(elements), index, length(elements),
           capacity(elements));

  auto gt = insert_at(children, index + 1, greater);
  assertx(gt);
  assertx(*gt == greater);
  return result;
}

//=====================================
template <typename T, std::size_t keys, typename Cmp>
static const T *
median(const sp::UinStaticArray<T, keys> &elements, const T *extra) noexcept {
#if 0
  // 0,1,2,3|4
  std::size_t mid = (length(elements) + 1) / 2;
  assertxs(mid < length(elements), mid);
  assertxs(mid > 0, mid);

  Cmp cmp;
  if (cmp(*extra, /*>*/ elements[mid])) {
    return elements.data() + mid;
  }

  ++mid;
  if (mid < length(elements)) {
    return elements.data() + mid;
  }

  return extra;
#endif
  // XXX:
  constexpr std::size_t max = keys + 1;
  sp::UinStaticArray<T, max> ptrs;
  Cmp cmp;
  {
    for (std::size_t i = 0; i < length(elements); ++i) {
      T *res = bin_insert(ptrs, elements[i], cmp);
      assertx(res);
    }
    // bool res = insert_all(ptrs, elements);
    assertxs(length(ptrs) == length(elements), length(ptrs), length(elements));
  }
  {
    bool res = sp::bin_insert(ptrs, *extra, cmp);
    assertx(res);
  }
  std::size_t mid = length(ptrs) / 2;
  const T &needle = ptrs[mid];

  const T *const result = sp::bin_search(elements, needle, cmp);
  if (result) {
    assertxs(!cmp(needle, *result) && !cmp(*result, needle), needle, *result);
    return result;
  }
  assertxs(!cmp(needle, *extra) && !cmp(*extra, needle), needle, *extra);

  return extra;
}

//=====================================
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
      auto src_gt = source_child[i + 1];
      T *const res = bin_insert(to, std::move(source[i]), src_gt);
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
    assertx(move);
  }

  // drop last X moved elements & children
  drop_back(from.elements, drop);
  drop_back(from.children, drop);
}

//=====================================
template <typename T, std::size_t keys, typename Cmp>
static void
extract(BTNode<T, keys, Cmp> &tree, const T &mid, T &dest,
        BTNode<T, keys, Cmp> &right) noexcept {
  Cmp cmp;
  const T *needle = bin_search(tree.elements, mid, cmp);
  assertx(needle);

  auto &elements = tree.elements;
  const std::size_t idx = index_of(elements, needle);
  assertxs(idx != capacity(elements), idx, length(elements));

  {
    bool res = stable_take(elements, idx, dest);
    assertx(res);
  }

  BTNode<T, keys, Cmp> *gt = nullptr;
  bool res = stable_take(tree.children, idx + 1, gt);
  assertx(res);

  // should be placed first in the right tree
  assertxs(length(right.children) >= 1, length(right.children));
  assertx(right.children[0] == nullptr);
  right.children[0] = gt;
}

//=====================================
template <typename T, std::size_t keys, typename Cmp>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
empty() noexcept {
  T *p = nullptr;
  BTNode<T, keys, Cmp> *r = nullptr;
  return std::make_tuple(p, r);
}

//=====================================
template <typename T, std::size_t keys, typename Cmp>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
fixup(BTNode<T, keys, Cmp> *const tree, T *bubble,
      BTNode<T, keys, Cmp> *const greater, T *&out) noexcept;

template <typename T, std::size_t keys, typename Cmp, typename Key>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
insert(BTNode<T, keys, Cmp> *const tree, Key &&needle, T *&out) noexcept {
  if (tree == nullptr) {
    auto bubble = new T(std::forward<Key>(needle));
    BTNode<T, keys, Cmp> *greater = nullptr;

    return std::make_tuple(bubble, greater);
  }

  auto &children = tree->children;
  auto &elements = tree->elements;

  /* 1. Traverse down */
  auto result = empty<T, keys, Cmp>();
  Cmp cmp;
  T *const gte = bin_find_gte(elements, needle, cmp);
  if (gte) {
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */
      out = gte;
      return empty<T, keys, Cmp>();
    }

    /* go down less-than $gte child */
    std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, capacity(elements));

    const auto child = children[index];
    result = insert(child, std::forward<Key>(needle), out);
  } else {
    assertx(!is_empty(*tree));

    /* go down the greater */
    auto child = last(children);
    assertxs(child, length(children));
    result = insert(*child, std::forward<Key>(needle), out);
  }

  /* 2. Fixup */
  T *const bubble = std::get<0>(result);
  if (bubble) {
    BTNode<T, keys, Cmp> *gt = std::get<1>(result);
    return fixup(tree, bubble, gt, out);
  }

  return empty<T, keys, Cmp>();
}

//=====================================
template <typename T, std::size_t keys, typename Cmp>
static std::tuple<T *, BTNode<T, keys, Cmp> *>
fixup(BTNode<T, keys, Cmp> *const tree, T *bubble,
      BTNode<T, keys, Cmp> *const greater, T *&out) noexcept {
  assertx(bubble);

  if (!is_full(*tree)) {
    T *res = bin_insert(*tree, std::move(*bubble), greater);
    assertx(res);

    if (!out) {
      out = res;
    }

    delete bubble;
    return empty<T, keys, Cmp>();
  }

  /* split tree into two */
  auto &elements = tree->elements;
  const T *const med = median<T, keys, Cmp>(elements, bubble);
  assertx(med);

  auto right = new BTNode<T, keys, Cmp>;
  assertx(right); // XXX

  partition(*tree, med, *right);

  if (med != bubble) {
    T med_copy = *med; // TODO fix this[insert will invalidate $med]
    // XXX: assert $m is last in $tree
    Cmp cmp;
    T *res = nullptr;
    if (cmp(*bubble, /*>*/ med_copy)) {
      res = bin_insert(*right, std::move(*bubble), greater);
    } else {
      res = bin_insert(*tree, std::move(*bubble), greater);
    }
    assertx(res);
    if (!out) {
      out = res;
    }

    extract(*tree, /*src*/ med_copy, /*dest*/ *bubble, *right);
  } else {
    assertx(right->children[0] == nullptr);
    right->children[0] = greater;
  }

  return std::make_tuple(bubble, right);
}
} // namespace impl

//=====================================
// TODO what is the log(order,elements) forumla to get the height?
template <typename T, std::size_t keys, typename Cmp, typename Key>
T *
insert(BTree<T, keys, Cmp> &self, Key &&value) noexcept {
  T *out = nullptr;
  auto result = impl::insert(self.root, std::forward<Key>(value), out);

  T *const bubble = std::get<0>(result);
  if (bubble) {
    BTNode<T, keys, Cmp> *const left = self.root;
    BTNode<T, keys, Cmp> *const right = std::get<1>(result);

    self.root = new BTNode<T, keys, Cmp>;
    assertx(self.root); // XXX
    {
      auto res = insert(self.root->children, left);
      assertx(res);
    }

    T *const res = impl::bin_insert(*self.root, std::move(*bubble), right);
    assertx(res);
    if (!out) {
      out = res;
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

  Cmp cmp;
  const T *const gte = bin_find_gte<T, keys, Key, Cmp>(elements, needle, cmp);
  if (gte) {
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */
      return gte;
    }

    /* Go down less than element child */
    std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, length(elements));

    const auto child = children[index];
    return find(child, needle);
  }

  /* needle is greater than any other element in elements */
  const auto child = last(children);
  assertxs(child, length(children));
  return find(*child, needle);
}
} // namespace impl

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
namespace impl {
/**/
template <typename T, std::size_t keys, typename Cmp>
static bool
is_leaf(const BTNode<T, keys, Cmp> &self) noexcept {
  return for_all(self.children, [](const auto &child) {
    /**/
    return child == nullptr;
  });
}

template <typename T, std::size_t keys, typename Cmp>
static bool
is_underflow(const BTNode<T, keys, Cmp> &self) noexcept {
  // TODO
  return false;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_wrapper(BTNode<T, keys, Cmp> &self, T *subject, Dest &dest) noexcept;

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_leaf(BTNode<T, keys, Cmp> &self, T *subject, Dest &dest) noexcept {
  assertx(is_leaf(self));

  auto &elements = self.elements;
  auto &children = self.children;

  const std::size_t index = index_of(elements, subject);
  assertxs(index != capacity(elements), index, length(elements));
  {
    bool res = stable_take(elements, index, dest);
    assertx(res);
  }
  {
    bool res = stable_remove(children, index + 1);
    assertx(res);
  }
  // TODO check if is_underflow
  return true;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_int_node(BTNode<T, keys, Cmp> &self, T *subject, Dest &dest) noexcept {
  auto &elements = self.elements;
  auto &children = self.children;

  const std::size_t index = index_of(elements, subject);
  assertxs(index != capacity(elements), index, length(elements));

  dest = std::move(*subject);

  BTNode<T, keys, Cmp> *const lt = children[index];
  if (lt) {
    assertx(!is_empty(*lt));
    T *const lt_sub = &lt->elements[0];
    return take_wrapper(*lt, lt_sub, *subject);
  }

  BTNode<T, keys, Cmp> *const gt = children[index + 1];
  if (gt) {
    assertx(!is_empty(*gt));
    T *const gt_sub = &gt->elements[0];
    return take_wrapper(*gt, gt_sub, *subject);
  }

  {
    bool res = stable_remove(elements, index);
    assertx(res);
  }
  {
    bool res = stable_remove(children, index + 1);
    assertx(res);
  }

  return true;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_wrapper(BTNode<T, keys, Cmp> &self, T *subject, Dest &dest) noexcept {
  if (is_leaf(self)) {
    return take_leaf(self, subject, dest);
  }
  return take_int_node(self, subject, dest);
}

template <typename T, std::size_t keys, typename Cmp, typename Key,
          typename Dest>
static bool
take(BTNode<T, keys, Cmp> *const tree, const Key &needle, Dest &dest) noexcept {
  if (tree == nullptr) {
    return false;
  }

  auto &children = tree->children;
  auto &elements = tree->elements;

  Cmp cmp;
  T *const gte = bin_find_gte<T, keys, Key, Cmp>(elements, needle, cmp);
  if (gte) {
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */
      return take_wrapper(*tree, gte, dest);
    }

    /* Go down less than element child */
    const std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, length(elements));

    auto child = children[index];
    return take(child, needle, dest);
  }

  /* needle is greater than any other element in elements */
  auto child = last(children);
  assertxs(child, length(children));
  return take(*child, needle, dest);
}

template <typename T>
struct BTreeNop {
  BTreeNop &
  operator=(T &&) noexcept {
    return *this;
  }
};
}

template <typename T, std::size_t keys, typename Comparator, typename Key>
bool
remove(BTree<T, keys, Comparator> &self, const Key &needle) noexcept {
  impl::BTreeNop<T> nop;
  return impl::take(self.root, needle, nop);
}

} // namespace rec
} // namespace sp

#endif
