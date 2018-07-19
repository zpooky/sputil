#ifndef SP_UTIL_TREE_BTREE_REC_H
#define SP_UTIL_TREE_BTREE_REC_H

#include <collection/Array.h>
#include <cstddef>
#include <sort/util.h>
#include <tuple>
#include <util/comparator.h>

// #define BTREE_REC_DEBUG

/*
 * # Knuth's definition
 * Knuth defines the order to be maximum number of children (which is one more
 * than the maximum number of keys).
 *
 * B-tree of order=m is a tree which satisfies the following properties:
 * - Every node has at most m children.
 * - Every non-leaf node (except root) has at least m/2 children.
 * - The root has at least two children if it is not a leaf node.
 * - A non-leaf node with k children contains k−1 keys.
 * - All leaves appear in the same level
 */

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

//=====================================
template <typename T, std::size_t keys, typename Comparator>
bool
is_empty(const BTree<T, keys, Comparator> &) noexcept;

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

    BTNode<T, keys, Cmp> *const less = nullptr;
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

  {
    BTNode<T, keys, Cmp> **const gt = insert_at(children, index + 1, greater);
    assertx(gt);
    assertx(*gt == greater);
  }

  return result;
}

template <typename T, std::size_t keys, typename Cmp, typename Key>
static T *
bin_insert(BTNode<T, keys, Cmp> &dest, BTNode<T, keys, Cmp> *less,
           Key &&value) noexcept {
  auto &children = dest.children;
  auto &elements = dest.elements;

  Cmp cmp;
  T *const result = bin_insert(elements, std::forward<Key>(value), cmp);
  assertxs(result, length(elements), capacity(elements), length(children),
           capacity(children));

  const std::size_t index = index_of(elements, result);
  assertxs(index != capacity(elements), index, length(elements),
           capacity(elements));

  {
    BTNode<T, keys, Cmp> **const l = insert_at(children, index, less);
    assertx(l);
    assertx(*l == less);
  }

  if (length(children) == 1) {
    assertx(length(elements) == 1);

    BTNode<T, keys, Cmp> *const greater = nullptr;
    auto res = insert(children, greater);
    assertx(res);
    assertx(*res == greater);
  }

  return result;
}

//=====================================
template <std::size_t keys>
static constexpr std::size_t
btree_middle() {
  std::size_t result = keys / 2;
  if (keys % 2 == 0) {
    --result;
  }
  return result;
}

template <typename T, std::size_t keys, typename Cmp>
static const T *
median(const sp::UinStaticArray<T, keys> &elements, const T *extra) noexcept {
  assertx(is_full(elements));
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
    assertx(is_full(ptrs));
  }
  assertxs(is_full(ptrs), length(ptrs), capacity(ptrs));
  std::size_t mid = btree_middle<max>();
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
      BTNode<T, keys, Cmp> *const bub_gt, T *&out) noexcept {
  assertx(tree);
  assertx(bubble);

  if (!is_full(*tree)) {
    T *res = bin_insert(*tree, std::move(*bubble), bub_gt);
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
    // XXX: assert $med is last in $tree
    Cmp cmp;
    T *res = nullptr;
    if (cmp(*bubble, /*>*/ med_copy)) {
      res = bin_insert(*right, std::move(*bubble), bub_gt);
    } else {
      res = bin_insert(*tree, std::move(*bubble), bub_gt);
    }
    assertx(res);
    if (!out) {
      out = res;
    }

    extract(*tree, /*src*/ med_copy, /*dest*/ *bubble, *right);
  } else {
    assertx(right->children[0] == nullptr);
    right->children[0] = bub_gt;
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
static constexpr std::size_t
minimum_order() noexcept {
  return BTNode<T, keys, Cmp>::order / 2;
}

template <typename T, std::size_t keys, typename Cmp>
static bool
is_deficient(const BTNode<T, keys, Cmp> &self) noexcept {
  const auto &elements = self.elements;
  return length(elements) < minimum_order<T, keys, Cmp>();
}

template <typename T, std::size_t keys, typename Cmp>
static bool
has_more_than_min(const BTNode<T, keys, Cmp> &self) noexcept {
  const auto &elements = self.elements;
  return length(elements) > minimum_order<T, keys, Cmp>();
}

// #ifdef BTREE_REC_DEBUG
template <typename T>
static void
du(T &elements) noexcept {
  printf("[");
  for (std::size_t i = 0; i < length(elements); ++i) {
    printf("%d,", elements[i]);
  }
  printf("]");
}
// #endif

template <typename T, std::size_t keys, typename Cmp>
static bool
merge(BTNode<T, keys, Cmp> &dest, BTNode<T, keys, Cmp> *src) noexcept {
  assertx(src);
  // assertx(is_leaf(*src));
  // assertx(is_leaf(dest));

  if (length(src->elements) > remaining_write(dest.elements)) {
    return false;
  }

  const bool result = move_all(dest.elements, src->elements);
  if (result) {
    auto &children = dest.children;
    {
      bool res = insert_all(children, src->children);
      assertx(res);
    }

    // assertxs(is_full(dest), length(dest.elements), length(dest.children));

    assertx(is_empty(src->elements));
#ifndef NDEBUG
    // TODO fix cmp to be in-line with is_sorted
    bool s = sp::is_sorted<T, sp::inverse<Cmp>>(dest.elements.data(),
                                                length(dest.elements));
    if (!s) {
      du(dest.elements);
      printf("\n");
    }
    assertx(s);
#endif

    { /**/
      clear(src->elements);
      clear(src->children);
      delete src;
    }
  }

  return result;
}

enum class ChildDir : bool { LEFT, RIGHT };

template <typename T, std::size_t keys, typename Cmp>
static bool
rebalance(BTNode<T, keys, Cmp> &parent, T *pivot, ChildDir dir) noexcept {
  assertx(!is_leaf(parent));
  assertx(pivot);

  auto &elements = parent.elements;
  auto &children = parent.children;

  const std::size_t piv_idx = index_of(elements, pivot);
  assertxs(piv_idx != capacity(elements), piv_idx, length(elements));

  const std::size_t left_idx = piv_idx;
  const std::size_t right_idx = piv_idx + 1;

  {
    if (dir == ChildDir::LEFT) {
      const std::size_t subject_idx = piv_idx;
      BTNode<T, keys, Cmp> *const subject = children[subject_idx];
      assertx(subject);

      /* # Rotation
       * before:
       *          [xxxx,pivot,yyyy]
       *         /     |     |     \
       *        ?     left  right   ?
       *               |     |
       *  [aa, nil, nil]     [bb, cc, dd]
       *  |   \             /   |    \   \
       * a0   a1          b0   b1    c1  d1
       *
       * after:
       *          [xxxx, bb ,yyyy]
       *         /     |    |     \
       *        ?     left right   ?
       *               |    |
       *  [aa,pivot,nil]    [cc, dd, nil]
       *  |  |     |       /   |    \
       * a0  a1    b0      b1  c1    d1
       *
       *
       * left of pivot is deficient:
       * 1. Copy the separator from the parent to the end of the deficient node
       *    (the separator moves down; the deficient node now has the minimum
       *    number of elements)
       * 2. Replace the separator in the parent with the first element of the
       *    right sibling (right sibling loses one node but still has at least
       *    the minimum number of elements)
       * 3. The tree is now balanced
       */

      BTNode<T, keys, Cmp> *const right_sibling = children[right_idx];
      if (right_sibling) {
        if (has_more_than_min(*right_sibling)) {
#ifdef BTREE_REC_DEBUG
          {
            printf("balance-left[pivot: %d]", *pivot);
            printf("rigt:");
            du(right_sibling->elements);
            printf("\n");
          }
#endif

          // 1.
          {
            BTNode<T, keys, Cmp> *right_smallest = right_sibling->children[0];
            T *res = bin_insert(*subject, std::move(*pivot), right_smallest);
            assertx(res);
          }

          // 2.
          {
            bool tres = stable_take(right_sibling->elements, 0, *pivot);
            assertx(tres);

            bool res = stable_remove(right_sibling->children, 0);
            assertx(res);
          }

          // 3. done with rebalance
          return false;
        }
      }

    } /*ChildDir::RIGHT*/ else {
      const std::size_t subject_idx = piv_idx + 1;
      BTNode<T, keys, Cmp> *const subject = children[subject_idx];
      assertx(subject);
      /* # Rotation:
       * before:
       *          [xxxx,pivot,yyyy]
       *         /     |     |     \
       *        ?     left  right   ?
       *               |     |
       *   [aa ,bb , cc]     [dd, nil, nil]
       *  /   /    |   |     |   \
       * a0  a1    b1  c1    d0   d1
       *
       * after:
       *          [xxxx, cc ,yyyy]
       *         /     |    |     \
       *        ?     left right   ?
       *               |    |
       *   [aa ,bb ,nil]    [pivot, dd, nil]
       *  /   /    |        |     |    \
       * a0  a1    b1       c1    d0    d1
       *
       * right of pivot is deficient
       *
       * 1. Copy the separator from the parent to the start of the deficient
       *    node (the separator moves down; deficient node now has the minimum
       *    number of elements)
       * 2. Replace the separator in the parent with the last element of the
       *    left sibling (left sibling loses one node but still has at least
       *    the minimum number of elements)
       * 3. The tree is now balanced
       */

      BTNode<T, keys, Cmp> *const left_sibling = children[left_idx];
      if (left_sibling) {
        if (has_more_than_min(*left_sibling)) {
#ifdef BTREE_REC_DEBUG
          {
            printf("balance-right[pivot: %d]", *pivot);
            printf("left:");
            du(left_sibling->elements);
            printf("\n");
          }
#endif

          // 1.
          {
            BTNode<T, keys, Cmp> **left_greatest = last(left_sibling->children);
            assertx(left_greatest);
            T *res = bin_insert(*subject, *left_greatest, std::move(*pivot));
            assertx(res);
          }

          // 2.
          {
            {
              std::size_t last_idx = length(left_sibling->elements) - 1;
              bool res = stable_take(left_sibling->elements, last_idx, *pivot);
              assertx(res);
            }
            std::size_t last_idx = length(left_sibling->children) - 1;
            bool res = stable_remove(left_sibling->children, last_idx);
            assertx(res);
          }

          // 3.done with rebalance
          return false;
        }
      }
    }
  }

  /* # Merge
   * before:
   *        [xx  ,pivot,  yy]
   *       /     |     |     \
   *      x0    left  right   y0
   *             |     |
   * [aa ,nil,nil]     [dd, nil, nil]
   * |    \            |   \
   * a0    a1          d0   d1
   *
   * after:
   *   [xx  , yy]
   *  /     |    \
   * x0     |     y0
   *        |
   *        [aa,pivot,dd]
   *       /   |     |   \
   *      a0   a1    d0   d1
   *
   * Otherwise, if both immediate siblings have only the minimum number of
   * elements, then merge with a sibling sandwiching their separator taken
   * off from their parent

   * 1. Copy the separator to the end of the left node (the left node may be
   *    the deficient node or it may be the sibling with the minimum number
   *    of elements)
   * 2. Move all elements from the right node to the left node (the left node
   *    now has the maximum number of elements, and the right node – empty)
   * 3. Remove the separator from the parent along with its empty right child
   *    (the parent loses an element)
   *    - If the parent is the root and now has no elements, then free it and
   *      make the merged node the new root (tree becomes shallower)
   *    - Otherwise, if the parent has fewer than the required number of
   *      elements, then rebalance the parent
   */

  BTNode<T, keys, Cmp> *const left_child = children[left_idx];
  BTNode<T, keys, Cmp> *const right_child = children[right_idx];
  {
    if (!left_child && !right_child) {
      assertx(false);
      return false;
    }

    bool cont = false;
    if (right_child && left_child) {
      // TODO define behaviour
      if (!is_deficient(*left_child) && !is_deficient(*right_child)) {
        return false;
      }

      const std::size_t req_len = (length(right_child->elements) + 1);
      if (remaining_write(left_child->elements) >= req_len) {
        cont = true;
      }
    }

    if (!cont) {
      if (left_child && is_empty(left_child->elements)) {
        /*delete left child node if empy*/
        assertx(is_leaf(*left_child));
        { /**/
          delete left_child;
        }
        children[left_idx] = nullptr;
      }

      /*delete right child node if empy*/
      if (right_child && is_empty(right_child->elements)) {
        assertx(is_leaf(*right_child));
        { /**/
          delete right_child;
        }
        children[right_idx] = nullptr;
      }
      return false;
    }
  }

#ifdef BTREE_REC_DEBUG
  {
    printf("reb-otherwise[pivot: %d]", *pivot);
    printf("left:");
    du(left_child->elements);
    printf("right:");
    du(right_child->elements);
    printf("\n");
  }
#endif

  // 1. move pivot into left
  {
    T *const res = insert(left_child->elements, std::move(*pivot));
    assertx(res);
    /* We explicitly do not add $gt child here since we will merge all children
     * from $right_child which is the old $gt of $pivot
     */
  }
  // 2. merge right into left and gc right
  {
    bool res = merge(/*DEST*/ *left_child, /*SRC->gc*/ right_child);
    assertx(res);
  }
  // 3. remove old pivot
  {
    {
      bool res = stable_remove(elements, piv_idx);
      assertx(res);
    }

    {
      bool res = stable_remove(children, piv_idx + 1);
      assertx(res);
    }
  }

  // continue rebalance
  return true;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_wrapper(BTNode<T, keys, Cmp> &, T *, Dest &dest) noexcept;

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

  return is_deficient(self);
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_max(BTNode<T, keys, Cmp> &self, Dest &dest) {
  if (is_leaf(self)) {
    T *const subject = last(self.elements);
    assertx(subject);
    return take_leaf(self, subject, dest);
  }

  const std::size_t max_idx = length(self.elements);
  BTNode<T, keys, Cmp> *const greatest = self.children[max_idx];
  if (greatest) {
    const bool balance = take_max(*greatest, dest);
    if (balance) {
      T *const pivot = &self.elements[max_idx - 1];
      /* $greatest is RIGHT of $pivot */
      return rebalance(self, pivot, ChildDir::RIGHT);
    }

    return false;
  }

  assertx(false);
  return false;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_min(BTNode<T, keys, Cmp> &self, Dest &dest) {
  const std::size_t smallest_idx = 0;

  if (is_leaf(self)) {
    T *const subject = &self.elements[smallest_idx];
    return take_leaf(self, subject, dest);
  }

  BTNode<T, keys, Cmp> *const smallest = self.children[smallest_idx];
  if (smallest) {
    const bool balance = take_min(*smallest, dest);
    if (balance) {
      T *const pivot = &self.elements[smallest_idx];
      /* $smallest is LEFT of $pivot */
      return rebalance(self, pivot, ChildDir::LEFT);
    }

    return false;
  }

  assertx(false);
  return false;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_internal_node(BTNode<T, keys, Cmp> &self, T *subject,
                   Dest &dest) noexcept {
  assertx(subject);
  auto &elements = self.elements;
  auto &children = self.children;

  const std::size_t index = index_of(elements, subject);
  assertxs(index != capacity(elements), index, length(elements));

  dest = std::move(*subject);

  BTNode<T, keys, Cmp> *const lt = children[index];
  if (lt) {
    assertxs(!is_empty(lt->elements), *subject);
    assertxs(!is_empty(lt->children), *subject);
    assertxs(!is_empty(*lt), *subject);

    const bool balance = take_max(*lt, *subject);
    if (balance) {
      return rebalance(self, subject, ChildDir::LEFT);
    }

    return false;
  }

  BTNode<T, keys, Cmp> *const gt = children[index + 1];
  if (gt) {
    assertxs(!is_empty(*gt), *subject);

    const bool balance = take_min(*gt, *subject);
    if (balance) {
      return rebalance(self, subject, ChildDir::RIGHT);
    }

    return false;
  }

  // should not get here
  assertx(false);
  return false;
}

template <typename T, std::size_t keys, typename Cmp, typename Dest>
static bool
take_wrapper(BTNode<T, keys, Cmp> &self, T *subject, Dest &dest) noexcept {
  if (is_leaf(self)) {
    return take_leaf(self, subject, dest);
  }

  return take_internal_node(self, subject, dest);
}

template <typename T, std::size_t keys, typename C, typename Key, typename Dest>
static bool
take(BTNode<T, keys, C> *const tree, const Key &needle, Dest &dest,
     bool &balance) noexcept {
  if (tree == nullptr) {
    balance = false;
    return false;
  }

  auto &children = tree->children;
  auto &elements = tree->elements;

  C cmp;
  T *const gte = bin_find_gte<T, keys, Key, C>(elements, needle, cmp);
  if (gte) {
    if (!cmp(needle, *gte) && !cmp(*gte, needle)) {
      /* equal */

      balance = take_wrapper(*tree, gte, dest);
      return true;
    }

    /* Go down less than element child */
    const std::size_t index = index_of(elements, gte);
    assertxs(index != capacity(elements), index, length(elements));

    auto child = children[index];
    const bool result = take(child, needle, dest, balance);
    if (balance) {
      balance = rebalance(*tree, /*pivot*/ gte, ChildDir::LEFT);
    }
    return result;
  }

  /* needle is greater than any other element in elements */
  BTNode<T, keys, C> **child = last(children);
  assertxs(child, length(children));
  const bool result = take(*child, needle, dest, balance);
  if (balance) {
    T *const pivot = last(elements);
    assertx(pivot);
    balance = rebalance(*tree, pivot, ChildDir::RIGHT);
  }

  return result;
}

template <typename T>
struct BTreeNop {
  BTreeNop &
  operator=(T &&) noexcept {
    return *this;
  }
};
} // namespace impl

template <typename T, std::size_t keys, typename Comparator, typename Key>
bool
remove(BTree<T, keys, Comparator> &self, const Key &needle) noexcept {
  impl::BTreeNop<T> nop;
  bool balance = false;
  const bool result = impl::take(self.root, needle, nop, balance);
  if (balance) {
    BTNode<T, keys, Comparator> *const old = self.root;
    auto &elements = old->elements;

    if (is_empty(elements)) {
      auto &children = old->children;

      assertx(length(children) == 1);
      self.root = children[0];
      {
        clear(children);
        clear(old->elements);
        delete old;
      }
    }
  }

  return result;
}

//=====================================
template <typename T, std::size_t keys, typename Comparator>
bool
is_empty(const BTree<T, keys, Comparator> &self) noexcept {
  return self.root == nullptr;
}

} // namespace rec
} // namespace sp

#endif
