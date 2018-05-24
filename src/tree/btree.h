#ifndef SP_UTIL_TREE_BTREE_H
#define SP_UTIL_TREE_BTREE_H

#include <collection/Array.h>
#include <tuple>
#include <util/assert.h>

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
//=====================================
template <typename T, std::size_t k>
struct BTNode {
  using value_type = T;
  static constexpr std::size_t keys = k;
  static constexpr std::size_t order = keys + 1;

  BTNode<T, keys> *parent;
  sp::UinStaticArray<T, keys> elements;
  sp::UinStaticArray<BTNode<T, keys> *, keys + 1> children;

  BTNode(BTNode<T, keys> *p = nullptr) noexcept;

  BTNode(const BTNode<T, keys> &) = delete;
  BTNode(const BTNode<T, keys> &&) = delete;
  BTNode<T, keys> &
  operator=(const BTNode<T, keys> &) = delete;
  BTNode<T, keys> &
  operator=(const BTNode<T, keys> &&) = delete;
};

} // namespace btree
} // namespace impl

//=====================================
template <typename T, std::size_t keys, typename Comparator = sp::greater>
struct Tree {
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
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

//=====================================
template <typename T, std::size_t k, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, k, C> &, K &&) noexcept;

//=====================================
template <typename T, std::size_t k, typename C, typename K>
const T *
find(const Tree<T, k, C> &, const K &) noexcept;

template <typename T, std::size_t k, typename C, typename K>
T *
find(Tree<T, k, C> &, const K &) noexcept;

//=====================================

/*
 * template <typename T, typename C>
 * bool
 * verify(Tree<T, C> &tree) noexcept;
 */

//=====================================
//====Implementation===================
//=====================================
namespace impl {
namespace btree {
//=====================================
template <typename T, std::size_t keys>
BTNode<T, keys>::BTNode(BTNode<T, keys> *p) noexcept
    : parent{p}
    , elements()
    , children() {
}

//=====================================
template <std::size_t K>
static constexpr std::size_t
median() noexcept {
  return K / 2;
}

//=====================================
template <typename T, std::size_t K>
static bool
is_full(const BTNode<T, K> &tree) noexcept {
  bool res = is_full(tree.elements);
  if (res) {
    assertx(is_full(tree.children));
  }
  return res;
}

//=====================================
template <typename T, std::size_t K>
static bool
is_empty(const BTNode<T, K> &tree) noexcept {
  return is_empty(tree.elements);
}

// template <typename T, std::size_t K, typename V>
// static T *
// do_insert_node(BTNode<T, K> *left, V &&v) noexcept {
//   return nullptr;
// }

//=====================================
template <typename T, std::size_t K>
static void
copy_split(BTNode<T, K> *left, std::size_t sep, BTNode<T, K> *right) noexcept {
  assertx(left);
  assertx(right);
  assertx(is_empty(*right));
  assertx(left->parent == right->parent);

  /* TODO Always add the smaller than first element child pointer? */
  insert(right->children, nullptr);

  const std::size_t start = sep + 1;
  {
    /*
     * Move elements from @ the right of the median to the right subtree
     */
    std::size_t moved = 0;
    auto &elems = left->elements;
    for (std::size_t i = start; i < length(elems); ++i) {
      auto ins = insert(right->elements, std::move(elems[i]));
      assertx(ins);
      ++moved;
    }
    printf("e-moved[%zu]\n", moved);
    drop_back(right->elements, moved);
  }
  {
    /*
     * Move children from the left of the median to the right subtree
     */
    std::size_t moved = 0;
    auto &children = left->children;
    for (std::size_t i = start; i < length(children); ++i) {
      BTNode<T, K> **ins = insert(right->children, std::move(children[i]));
      assertx(ins);
      if (*ins) {
        /* Since it is not required that the node has a child */
        (*ins)->parent = right;
      }
      ++moved;
    }
    printf("c-moved[%zu]\n", moved);
    drop_back(right->children, moved);
  }
}

template <typename T, std::size_t K, typename V>
static T *
insert_parent_after(BTNode<T, K> *needle, V &&val,
                    BTNode<T, K> *right) noexcept {
  assertx(needle);
  assertx(right);
  auto parent = needle->parent;
  assertx(parent);
  assertx(!is_full(*parent));
  assertx(needle->parent == right->parent);

  auto res = find(parent->children,
                  [&needle](auto current) { //
                    return current == needle;
                  });
  assertx(res);
  auto idx = index_of(parent->children, res);
  assertx(idx != parent->children.capacity);

  auto elem_res = insert_at(parent->elements, idx, std::forward<V>(val));
  assertx(elem_res);
  ++idx;
  assertx(insert_at(parent->children, idx, right));

  printf("ins_p_a\n");
  return elem_res;
}

template <typename T, std::size_t K, typename V>
T *
do_insert_node(BTNode<T, K> *current, V &&val, BTNode<T, K> *gt) noexcept {
  assertx(current);
  assertx(!is_full(*current));

  auto res = bin_insert(current->elements, std::forward<V>(val));
  assertx(res);

  std::size_t idx = index_of(current->elements, res);
  assertx(idx != current->elements.capacity);

  ++idx;
  assertx(insert_at(current->children, idx, gt));
  return res;
}

template <typename T, std::size_t k, typename Cmp, typename V>
static std::tuple<BTNode<T, k> *, T *>
insert_node(BTNode<T, k> *tree, V &&val, std::size_t index,
            BTNode<T, k> *gt) noexcept;

template <typename T, std::size_t K, typename Cmp, typename V>
static std::tuple<BTNode<T, K> *, T *>
split_insert(BTNode<T, K> *tree, V &&val, BTNode<T, K> *gt = nullptr) noexcept {
#if 0
  assertx(tree);
  assertx(is_full(*tree));

Lit:
  auto parent = tree->parent;
  if (parent == nullptr) {
    /* Create a new parent since we are the root */
    parent = new BTNode<T, K>;
    if (parent == nullptr) {
      return std::make_pair(nullptr, nullptr);
    }
    insert(parent->children, tree);
    tree->parent = parent;
  }

  /* - Find the median of the full node.
   * - Create a new leaf node and copy into it all the keys which appear after
   *   the median.
   * - Move up the median at an appropriate position in the parent of this
   *   node.
   * - Add an additional child pointer (after the median) from the parent node
   *   to the new node.
   * - Add the new key at the right location in the child nodes of the median.
   */
  auto right = new BTNode<T, K>(parent);
  if (!right) {
    return std::make_pair(nullptr, nullptr);
  }
  /* copy everything at the right of median from the left subtree too the
   * empty right subtree
   */
  auto left = tree;
  std::size_t med = median<K>();
  copy_split(left, med, right);

  /* median is now at the end of left subtree and we need to bubble it up
   * together with right child onto the parent elements&child list
   */
  auto find_res = find(parent->children,
                       [&left](auto current) { //
                         return current == left;
                       });
  assertx(find_res);
  auto idx = index_of(parent->children, find_res);
  assertx(idx != capacity(parent->children));
  // auto rmed = insert_parent_after(left, std::move(left->elements[med]),
  // right);
  auto asda = insert_node<T, K, Cmp, V>(parent, std::move(left->elements[med]),
                                        idx, right);
  auto rmed = std::get<1>(asda);
  assertx(rmed);
  parent = std::get<0>(asda);
  assertx(parent);
  drop_back(left->elements, 1);

  // assertx(length(left->elements) + 1 == length(left->children));
  // assertx(length(right->elements) + 1 == length(right->children));

  /* actual perform the insertion of the new /val/ */
  Cmp c;
  T *res = nullptr;
  if (c(val, *rmed)) {
    res = do_insert_node(right, std::forward<V>(val), gt);
  } else {
    res = do_insert_node(left, std::forward<V>(val), gt);
  }
  assertx(res);

  return std::make_pair(parent, res);
#endif
  return std::make_tuple(nullptr, nullptr);
}

template <typename T, std::size_t k, typename Cmp, typename V>
static std::tuple<BTNode<T, k> *, T *>
insert_node(BTNode<T, k> *tree, V &&val, std::size_t index,
            BTNode<T, k> *gt) noexcept {
  assertx(tree);

  if (is_full(*tree)) {
    // dump(tree);
    return split_insert<T, k, Cmp, V>(tree, std::forward<V>(val), gt);
  } else {
    auto res = insert_at(tree->elements, index, std::forward<V>(val));
    assertx(res);

    {
      auto ires = insert_at(tree->children, index + 1, gt);
      assertx(ires);
    }
    return std::make_tuple(tree, res);
  }
}

} // namespace btree
} // namespace impl

//=====================================
template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::Tree() noexcept
    : root(nullptr) {
}

template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::~Tree() noexcept {
  // TODO
}

//=====================================
// http://btechsmartclass.com/DS/U5_T3.html
template <typename T, std::size_t k, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, k, C> &tree, K &&val) noexcept {
  using namespace btree::impl::btree;

  /* empty tree */
  if (tree.root == nullptr) {

    auto node = tree.root = new BTNode<T, k>(nullptr);
    if (node) {
      auto &elements = node->elements;
      auto res = bin_insert(elements, std::forward<K>(val));
      assertx(res);
      {
        auto ires = insert(node->children, /*lt*/ nullptr);
        assertx(ires);
      }
      {
        auto ires = insert(node->children, /*gt*/ nullptr);
        assertx(ires);
      }
      return std::make_tuple(res, true);
    }

    return std::make_tuple(nullptr, false);
  }

  auto current = tree.root;
Lit:
  auto &elements = current->elements;

  /* First node with value >= /val/ */
  T *const successor = sp::bin_find_gte<T, k, K, C>(elements, val);
  std::size_t index = length(elements);
  if (successor) {
    C cmp;
    if (!cmp(*successor, val) && !cmp(val, *successor)) {
      /* /val/ already present in node */
      return std::make_tuple(successor, false);
    }

    index = index_of(elements, successor);
    assertx(index != capacity(elements));

    if (*get(current->children, index)) {
      /* going down /successor/ less than child subtree*/
      current = *get(current->children, index);
      goto Lit;
    }
  } /*successor*/ else {
    /* /val/ is greater than any value in the current node */
    auto &children = current->children;
    if (*last(children)) {
      /* going down the greater than any element in this node subtree */
      current = *last(children);
      goto Lit;
    }
  }

  {
    /* insert /val/ potentially rebalancing the tree */
    auto sres =
        insert_node<T, k, C, K>(current, std::forward<K>(val), index, nullptr);
    auto p = std::get<0>(sres);
    if (p->parent == nullptr) {
      tree.root = p;
    }
    T *ins = std::get<1>(sres);
    return std::make_tuple(ins, ins != nullptr);
  }
}

//=====================================
template <typename T, std::size_t k, typename C, typename K>
const T *
find(const Tree<T, k, C> &tree, const K &needle) noexcept {
  const auto *node = tree.root;

// TODO bin search
Lit:
  if (node) {
    const auto &elems = node->elements;

    std::size_t idx = 0;
    for (; idx < elems.length; ++idx) {
      C cmp;
      const auto *current = sp::get(elems, idx);
      if /*it > needle*/ (cmp(*current, needle)) {
        break;
      } /*it < needle*/ else if (cmp(needle, *current)) {
      } else {
        /*
         * match !(needle <> current)
         */
        return current;
      }
    }
    /* Check that we are not in leaf node */
    const auto next = get(node->children, idx);
    if (next) {
      node = *next;
      goto Lit;
    }
  }

  return nullptr;
}

template <typename T, std::size_t k, typename C, typename K>
T *
find(Tree<T, k, C> &tree, const K &needle) noexcept {
  const auto &c_tree = tree;
  return (T *)find(c_tree, needle);
}
//=====================================

} // namespace btree
#endif
