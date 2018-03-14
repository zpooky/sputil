#ifndef SP_UTIL_TREE_BTREE_H
#define SP_UTIL_TREE_BTREE_H

#include <collection/Array.h>
#include <tuple>

// TODO array bin_insert comparator
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
  static constexpr std::size_t no_keys = keys;

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
 */

template <std::size_t k, typename C>
void
dump(const Tree<int, k, C> &) noexcept;

/*
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
BTNode<T, keys>::BTNode(BTNode<T, keys> *p) noexcept
    : parent{p}
    , elements()
    , children() {
}

template <typename std::size_t K>
constexpr std::size_t
median() noexcept {
  return K / 2;
}

template <typename T, std::size_t K>
static bool
is_full(const BTNode<T, K> &tree) noexcept {
  bool res = is_full(tree.elements);
  if (res) {
    assert(is_full(tree.children));
  }
  return res;
}

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

template <typename T, std::size_t K>
static void
copy_split(BTNode<T, K> *left, std::size_t sep, BTNode<T, K> *right) noexcept {
  assert(is_empty(*right));
  assert(left->parent == right->parent);

  /* TODO Always add the smaller than first element child pointer? */
  insert(right->children, nullptr);

  const std::size_t start = sep + 1;
  {
    /*
     * Move elements from the right of the median to the right subtree
     */
    std::size_t moved = 0;
    auto &elems = left->elements;
    for (std::size_t i = start; i < length(elems); ++i) {
      auto ins = insert(right->elements, std::move(elems[i]));
      assert(ins);
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
      assert(ins);
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
  assert(needle);
  assert(right);
  auto parent = needle->parent;
  assert(parent);
  assert(!is_full(*parent));
  assert(needle->parent == right->parent);

  auto res = find(parent->children,
                  [&needle](auto current) { //
                    return current == needle;
                  });
  assert(res);
  auto idx = index_of(parent->children, res);
  assert(idx != parent->children.capacity);

  auto elem_res = insert_at(parent->elements, idx, std::forward<V>(val));
  assert(elem_res);
  ++idx;
  assert(insert_at(parent->children, idx, right));

  printf("ins_p_a\n");
  return elem_res;
}

template <typename T, std::size_t K, typename V>
T *
do_insert_node(BTNode<T, K> *current, V &&val) noexcept {
  assert(current);
  assert(!is_full(*current));

  auto res = bin_insert(current->elements, std::forward<V>(val));
  assert(res);

  std::size_t idx = index_of(current->elements, res);
  assert(idx != current->elements.capacity);

  ++idx;
  assert(insert_at(current->children, idx, nullptr));
  return res;
}

template <typename T, std::size_t K, typename Cmp, typename V>
static std::tuple<BTNode<T, K> *, T *>
split_insert(BTNode<T, K> *tree, V &&val) noexcept {
  assert(tree);
  assert(is_full(*tree));

Lit:
  auto parent = tree->parent;
  if (parent && is_full(*parent)) {
    assert(false);
  } else {
    auto left = tree;
    if (!parent) {
      assert(!tree->parent);

      /* Create a new parent since we are the root */
      parent = new BTNode<T, K>;
      if (!parent) {
        return std::make_pair(nullptr, nullptr);
      }
      insert(parent->children, left);
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
    std::size_t med = median<K>();
    copy_split(left, med, right);

    /* median is now at the end of left subtree and we need to bubble it up
     * together with right child onto the parent elements&child list
     */
    auto res_med =
        insert_parent_after(left, std::move(left->elements[med]), right);
    assert(res_med);
    drop_back(left->elements, 1);

    // assert(length(left->elements) + 1 == length(left->children));
    // assert(length(right->elements) + 1 == length(right->children));

    /* actual perform the insertion of the new /val/ */
    Cmp c;
    T *res = nullptr;
    if (c(val, *res_med)) {
      res = do_insert_node(right, std::forward<V>(val));
    } else {
      res = do_insert_node(left, std::forward<V>(val));
    }
    assert(res);

    return std::make_pair(parent, res);
  }

  return std::make_pair(nullptr, nullptr);
}

} // namespace btree
} // namespace impl

template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::Tree() noexcept
    : root(nullptr) {
}

template <typename T, std::size_t keys, typename C>
Tree<T, keys, C>::~Tree() noexcept {
  // TODO
}

// http://btechsmartclass.com/DS/U5_T3.html
template <typename T, std::size_t k, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, k, C> &tree, K &&val) noexcept {
  using namespace btree::impl::btree;

  if (tree.root) {
    auto current = tree.root;
  Lit:
    if (current) {
      auto &elements = current->elements;

      /* First node with value >= /val/ */
      T *const successor = sp::bin_find_successor<T, k, K, C>(elements, val);
      if (successor) {
        C cmp;
        if (!cmp(*successor, val) && !cmp(val, *successor)) {
          /* /val/ already present in node */
          return std::make_tuple(successor, false);
        }

        std::size_t index = index_of(elements, successor);
        assert(index != capacity(elements));

        if (*get(current->children, index)) {
          current = *get(current->children, index);
          goto Lit;
        } else {
          if (is_full(elements)) {
            assert(is_full(current->children));
            assert(false);
            // TODO
          } else {
            auto res = insert_at(elements, index, std::forward<K>(val));
            assert(res);
            assert(insert_at(current->children, index+1, nullptr));
            return std::make_tuple(res, true);
          }
        }

      } /*successor*/ else {
        /* /val/ is greater than any value in the current node */
        auto &children = current->children;
        if (*last(children)) {
          current = *last(children);
          goto Lit;
        } else if (is_full(elements)) {
          assert(is_full(children));
          auto sres = split_insert<T, k, C, K>(current, std::forward<K>(val));
          auto p = std::get<0>(sres);
          if (p->parent == nullptr) {
            tree.root = p;
          }
          T *ins = std::get<1>(sres);
          return std::make_tuple(ins, ins != nullptr);
        } else {

          auto res = do_insert_node(current, std::forward<K>(val));
          return std::make_tuple(res, true);
        }
      }
    }

  } else {
    /* empty tree */
    auto node = tree.root = new BTNode<T, k>(nullptr);
    if (node) {
      auto &elements = node->elements;
      auto res = bin_insert(elements, std::forward<K>(val));
      assert(res);
      assert(insert(node->children, nullptr));
      assert(insert(node->children, nullptr));
      return std::make_tuple(res, true);
    }
  }
  return std::make_tuple(nullptr, false);
}

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

template <typename T>
static void
dump_elements(T *root) noexcept {
  printf("[");
  for (std::size_t i = 0; i < capacity(root->elements); ++i) {
    if (i != 0) {
      printf("|");
    }
    if (i < length(root->elements)) {
      printf("%02d", root->elements[i]);
    } else {
      printf("__");
    }
  }
  printf("]\n");
}

template <typename T>
static void
dump_children(T *root) noexcept {
  int height = 0;
  height = reduce(root->children, height, [](auto h, auto c) {
    if (c) {
      return h + 1;
    }
    return h;
  });
  int raw[T::no_keys] = {-1};
  for (std::size_t i = 0; i < length(root->children); ++i) {
    if (root->children[i]) {
      raw[i] = height--;
    }
  }
}

template <std::size_t keys>
void
dump(btree::impl::btree::BTNode<int, keys> *root) noexcept {
  if (root) {
    dump_elements(root);
    // dump_children(root);
    for_each(root->children, [](auto c) { //
      dump(c);
    });
  }
}

template <std::size_t k, typename C>
void
dump(const Tree<int, k, C> &tree) noexcept {
  dump(tree.root);
}

} // namespace btree
#endif
