#ifndef SP_TREE_BST_TREE_H
#define SP_TREE_BST_TREE_H

#include <string>
#include <tuple>
#include <util/assert.h>
#include <util/comparator.h>

namespace bst {
//=====================================
// typename Allocator = std::allocator<Key>
template <typename T, typename Comparator>
struct Tree {
  using value_type = typename T::value_type;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;

  T *root;

  Tree(T *r) noexcept
      : root(r) {
  }

  Tree() noexcept
      : Tree(nullptr) {
  }

  Tree(const Tree<T, Comparator> &) = delete;
  Tree(const Tree<T, Comparator> &&) = delete;

  Tree &
  operator=(const Tree<T, Comparator> &) = delete;
  Tree &
  operator=(const Tree<T, Comparator> &&) = delete;

  ~Tree() {
    if (root) {
      // TODO support non recursive delete
      delete root;
      root = nullptr;
    }
  }
};

//=====================================
template <typename T, typename C, typename S>
typename Tree<T, C>::const_pointer
find(const Tree<T, C> &tree, const S &search) noexcept;

template <typename T, typename C, typename S>
typename Tree<T, C>::pointer
find(Tree<T, C> &tree, const S &search) noexcept;

//=====================================
template <typename T, typename C>
void
swap(Tree<T, C> &, Tree<T, C> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
/*impl*/

//=====================================
template <typename T>
static bool
doubly_linked(T *n) noexcept {
  if (n) {
    bool l = n->left != nullptr ? n == n->left->parent : true;
    bool r = n->right != nullptr ? n == n->right->parent : true;

    return l && r;
  }
  return true;
}

//=====================================
template <typename T>
void
dump(T *tree, std::string prefix = "", bool isTail = true,
     const char *ctx = "") noexcept {
  if (tree) {
    char name[256] = {0};
    auto val = std::string(*tree);
    sprintf(name, "%s%s", ctx, val.c_str());

    printf("%s%s%s\n", prefix.c_str(), (isTail ? "└── " : "├── "), name);

    const char *ls = (isTail ? "    " : "│   ");
    if (tree->right && tree->left) {
      dump(tree->right, prefix + ls, false, "gt:");
      dump(tree->left, prefix + ls, true, "lt:");
    } else {
      if (tree->left) {
        dump(tree->left, prefix + ls, true, "lt:");
      } else if (tree->right) {
        dump(tree->right, prefix + ls, true, "gt:");
      }
    }
  }
}

//=====================================
template <typename T>
std::size_t
child_count(T *tree) noexcept {
  std::size_t result = 0;
  if (tree) {
    ++result;
    result += child_count(tree->left);
    result += child_count(tree->right);
  }
  return result;
}

//=====================================
/*
 * Recursively search down in the left branch to find the smallest node in the
 * tree.
 */
template <typename T>
T *
find_min(T *node) noexcept {
  assertx(node);

Lstart:
  if (node->left) {
    node = node->left;
    goto Lstart;
  }
  return node;
} // bst::impl::find_min()

//=====================================
/*
 * Recursivly search in tree until matching node is found
 */
template <typename N, typename C, typename K>
const N *
find_node(const Tree<N, C> &tree, const K &search) noexcept {
  const N *current = tree.root;
Lstart:
  if (current) {
    constexpr C cmp;
    if (cmp(current->value, /*>*/ search)) {

      current = current->left;
      goto Lstart;
    } else if (cmp(search, /*>*/ current->value)) {

      current = current->right;
      goto Lstart;
    }
  }

  return current;
} // bst::impl::find_node()

template <typename N, typename C, typename K>
static N *
find_node(Tree<N, C> &tree, const K &search) noexcept {
  const Tree<N, C> &c_tree = tree;
  return (N *)find_node(c_tree, search);
}

//=====================================
template <typename N, typename C, typename K>
static std::tuple<N *, bool>
insert(Tree<N, C> &tree, K &&ins) noexcept {
  if (tree.root == nullptr) {
    // insert into empty tree
    tree.root = new (std::nothrow) N(std::forward<K>(ins));
    if (tree.root) {
      return std::make_tuple(tree.root, true);
    }

    return std::make_tuple(nullptr, false);
  }

  N *it = tree.root;
Lit:
  constexpr C cmp;
  if (cmp(it->value, /*>*/ ins)) {
    if (it->left) {
      it = it->left;

      goto Lit;
    }

    it->left = new (std::nothrow) N(std::forward<K>(ins), it);
    if (it->left) {
      return std::make_tuple(it->left, true);
    }
  } else if (cmp(ins, /*>*/ it->value)) {
    if (it->right) {
      it = it->right;

      goto Lit;
    }

    it->right = new (std::nothrow) N(std::forward<K>(ins), it);
    if (it->right) {
      return std::make_tuple(it->right, true);
    }
  } else {

    return std::make_tuple(it, false);
  }

  return std::make_tuple(nullptr, false);
} // bst::impl::insert()

//=====================================
template <typename N>
static N *
remove(N *const current) noexcept {
  assertx(current);

  auto update_ParentToChild = [](N *subject, N *replacement) {
    // update parent -> child
    N *const parent = subject->parent;
    if (parent) {
      if (parent->left == subject) {
        parent->left = replacement;
      } else {
        assertx(parent->right == subject);
        parent->right = replacement;
      }
    }
  };

  auto unset = [](N *subject) {
    subject->parent = nullptr;
    subject->left = nullptr;
    subject->right = nullptr;
  };

  assertx(doubly_linked(current));

  if /*two children*/ (current->left && current->right) {
    /*
     * Replace $current with the minimum child found in $current right
     * branch($successor).
     * The $successor is the natural replacement for $current since all BST
     * properties will still hold after the remove:
     * - $successor will be greater than any node in the left branch since
     * $successor was taken from the right branch.
     * - $successor will be less than the node on the right since $successor was
     * the smallest node in the right branch.
     */
    N *const successor = find_min(current->right);
    {
      /*
       * Unlinks $successor.
       */
      remove(successor);

      /*
       * Update: [parent -> $current] Link. now: [parent -> $successor]
       */
      update_ParentToChild(current, successor);
      {
        /*
         * Update $successor relation pointers the be the same as $current
         */
        successor->parent = current->parent;
        successor->left = current->left;
        successor->left->parent = successor;

        if (current->right) {
          /*
           * If the immediate right was not picked as the successor(non-null).
           */
          successor->right = current->right;
          successor->right->parent = successor;
        }
      }

      assertx(doubly_linked(successor));
    }

    unset(current);

    /*
     * Returns the new root of the sub-Tree
     */
    return successor;
  } else if /*zero children*/ (!current->left && !current->right) {
    /*
     * Unset $current from [parent -> child] and replace it with null
     */
    N *const parent = current->parent;
    update_ParentToChild(current, (N *)nullptr);
    assertx(doubly_linked(parent));
    unset(current);

    return parent;
  } else if /*left child*/ (current->left) {
    /*
     * Exchange [parent -> $current] with [$parent -> $current->left]
     */
    N *const parent = current->parent;
    N *const left = current->left;
    update_ParentToChild(current, left);
    left->parent = parent;

    assertx(doubly_linked(parent));
    unset(current);

    return left;
  }
  assertx(current->right);
  /* right child */

  /*
   * Exchange [parent -> $current] with [$parent -> $current->right]
   */
  N *const parent = current->parent;
  N *const right = current->right;
  update_ParentToChild(current, right);
  right->parent = parent;

  assertx(doubly_linked(parent));
  unset(current);

  return right;
} // impl::remove()

} // namespace impl
//=====================================
template <typename N, typename C, typename K>
typename Tree<N, C>::const_pointer
find(const Tree<N, C> &tree, const K &search) noexcept {
  const N *const result = bst::impl::find_node(tree, search);
  if (result) {
    return &result->value;
  }
  return nullptr;
} // bst::find()

template <typename T, typename C, typename K>
typename Tree<T, C>::pointer
find(Tree<T, C> &tree, const K &search) noexcept {
  const Tree<T, C> &ctree = tree;
  return (typename Tree<T, C>::pointer)find<T, C, K>(ctree, search);
} // bst::find()

//=====================================
template <typename T, typename C>
void
swap(Tree<T, C> &first, Tree<T, C> &second) noexcept {
  using std::swap;
  swap(first.root, second.root);
} // bst::swap()

//=====================================

} // namespace bst
#endif
