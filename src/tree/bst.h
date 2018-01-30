#ifndef SP_MALLOC_BINARY_SEARCH_TREE_H
#define SP_MALLOC_BINARY_SEARCH_TREE_H

#include "tree.h"

namespace binary {
template <typename T>
struct Node {
  using value_type = T;

  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;
  T value;

  template <typename K>
  explicit Node(K &&k, Node<T> *p = nullptr)
      : left(nullptr)
      , right(nullptr)
      , parent(p)
      , value(std::forward<K>(k)) {
  }

  explicit operator std::string() const {
    std::string s;
    s.append("[v:");
    s.append(std::to_string(int(value)));
    s.append("]");
    return s;
  }

  ~Node() noexcept {
    // TODO this is recursive
    if (left) {
      delete left;
      left = nullptr;
    }
    if (right) {
      delete right;
      right = nullptr;
    }
  }
};

template <typename T, typename Comparator = sp::greater>
using Tree = bst::Tree<Node<T>, Comparator>;

template <typename T,typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T,C> &, K &&) noexcept;

template <typename T,typename C , typename K>
bool
remove(Tree<T,C> &, const K &) noexcept;

template <typename T,typename C>
void
dump(Tree<T,C> &tree, std::string prefix = "") noexcept;

template <typename T,typename C>
bool
verify(Tree<T,C> &tree) noexcept;

// Unbalanced Tree -> Balanced Tree
template <typename T,typename C>
bool
balance(Tree<T,C> &) noexcept;

//??
template <typename T,typename C>
bool
reverse(Tree<T, C> &) noexcept;

/*
 * ==========================================================================
 */
namespace impl {
namespace binary {

template <typename T>
bool
verify(Node<T> *parent, Node<T> *tree) noexcept {
  if (tree) {
    if (!verify(tree, tree->left)) {
      printf("left-");
      return false;
    }
    if (!verify(tree, tree->right)) {
      printf("right-");
      return false;
    }

    if (tree->parent != parent) {
      return false;
    }

    auto *left = tree->left;
    if (left) {
      if (!(tree->value > left->value)) {
        printf("left-");
        return false;
      }
    }

    auto *right = tree->right;
    if (right) {
      if (!(tree->value < right->value)) {
        printf("right-");
        return false;
      }
    }
  }
  return true;
} // impl::binary::verify()

} // namespace binary
} // namespace impl
//===================================================

template <typename T,typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T,C> &tree, K &&value) noexcept {
  auto result = bst::impl::insert(tree, std::forward<K>(value));
  Node<T>* node = std::get<0>(result);
  bool status = std::get<1>(result);
  T* val = nullptr;
  if(node){
    val = &node->value;
  }
  return std::make_tuple(val, status);
}

template <typename T,typename C, typename K>
bool
remove(Tree<T,C> &tree, const K &k) noexcept {
  auto set_root = [&tree](Node<T> *root) {
    if (root) {
      if(root->parent == nullptr) {
        tree.root = root;
      }
    } else {
      tree.root = nullptr;
    }
  };

  Node<T> *const node = bst::impl::find_node(tree, k);
  if (node) {

    Node<T> *const root = bst::impl::remove(node);
    set_root(root);

    delete (node);
    return true;
  }

  return false;
} // binary::remove()

template <typename T,typename C>
void
dump(Tree<T,C> &tree, std::string prefix) noexcept {
  return bst::impl::dump(tree.root, prefix);
} // binary::dump()

template <typename T,typename C>
bool
verify(Tree<T,C> &tree) noexcept {
  return impl::binary::verify((Node<T> *)nullptr, tree.root);
} // binary::verify()

// Unbalanced Tree -> Balanced Tree
template <typename T,typename C>
bool
balance(Tree<T,C> &) noexcept {
  // TODO
  return true;
} // binary::balance()

} // namespace bst

#endif
