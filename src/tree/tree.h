#ifndef SP_TREE_BST_TREE_H
#define SP_TREE_BST_TREE_H

#include <cassert>
#include <string>
#include <util/comparator.h>
#include <tuple>

namespace bst {
//typename Allocator = std::allocator<Key>
template <typename T, typename Comparator>
struct Tree {
  using value_type = typename T::value_type;
  using reference = value_type &;
  using const_reference = const reference;
  using pointer = value_type *;
  using const_pointer = const pointer;

  T *root;

  Tree() noexcept
      : root(nullptr) {
  }

  Tree(const Tree<T,Comparator> &) = delete;
  Tree(const Tree<T,Comparator> &&) = delete;

  Tree &
  operator=(const Tree<T,Comparator> &) = delete;
  Tree &
  operator=(const Tree<T,Comparator> &&) = delete;

  ~Tree() {
    if (root) {
      // TODO support non recursive delete
      delete root;
      root = nullptr;
    }
  }
};

template <typename T,typename C, typename S>
typename Tree<T,C>::const_pointer
find(const Tree<T,C> &tree, const S &search) noexcept;

template <typename T,typename C, typename S>
typename Tree<T,C>::pointer
find(Tree<T,C> &tree, const S &search) noexcept;

template <typename T,typename C>
void
swap(Tree<T,C> &, Tree<T,C> &) noexcept;

// TODO
// bfs_search
// inorder_search

/*
 * ==========================================================================
 */
namespace impl {
/*impl*/

/*
 * Recursively search down in the left branch to find the minimum
 * node in the tree.
 */
template <typename T>
T *
find_min(T *node) noexcept {
  assert(node);

Lstart:
  if (node->left) {
    node = node->left;
    goto Lstart;
  }
  return node;
} // bst::impl::find_min()

/*
 * Recursivly search in tree until matching node is found
 */
template <typename T, typename C, typename K>
T *
find_node(T *current, const K &search) noexcept {
Lstart:
  if (current) {
    constexpr C cmp;
    if /*current > search*/(cmp(current->value, search)) {

      current = current->left;
      goto Lstart;
    } else if /*current < search*/(cmp(search, current->value)) {

      current = current->right;
      goto Lstart;
    }
  }

  return current;
} // bst::impl::find_node()

template <typename N,typename C, typename K>
std::tuple<N *, bool>
insert(Tree<N, C> &tree, K &&ins) noexcept {
  if (!tree.root) {
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
  if /*ins < it->value*/(cmp(it->value,ins)) {
    if (it->left) {
      it = it->left;

      goto Lit;
    }

    it->left = new (std::nothrow) N(std::forward<K>(ins), it);
    if (it->left) {
      return std::make_tuple(it->left, true);
    }
  } else if /*ins > it->value*/(cmp(ins,it->value)) {
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

} // namespace impl
//===================================================

namespace impl {
namespace tree {

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

} // namespace tree
} // namespace impl

template <typename T,typename C, typename K>
typename Tree<T,C>::const_pointer
find(const Tree<T,C> &tree, const K &search) noexcept {
  auto *root = tree.root;
  auto *result = impl::find_node<T,C,K>(root,search);
  if (result) {
    return &result->value;
  }
  return nullptr;
}//bst::find()

template <typename T,typename C, typename K>
typename Tree<T,C>::pointer
find(Tree<T,C> &tree, const K &search) noexcept {
  const Tree<T,C> &ctree = tree;
  return (typename Tree<T,C>::pointer)find<T,C, K>(ctree, search);
}//bst::find()

template <typename T,typename C>
void
swap(Tree<T,C> &first, Tree<T,C> &second) noexcept {
  using std::swap;
  swap(first.root, second.root);
} // bst::swap()

} // namespace bst
#endif
