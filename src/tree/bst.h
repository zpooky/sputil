#ifndef SP_UTIL_TREE_BINARY_SEARCH_TREE_H
#define SP_UTIL_TREE_BINARY_SEARCH_TREE_H

#include <tree/tree.h>
#include <string>

namespace binary {
//=====================================
template <typename T>
struct Node {
  using value_type = T;

  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;
  T value;

  template <typename K>
  explicit Node(K &&, Node<T> * = nullptr) noexcept;

  explicit operator std::string() const;

  ~Node() noexcept;
};

template <typename T, typename Comparator = sp::greater>
using Tree = bst::Tree<Node<T>, Comparator>;

//=====================================
template <typename T, typename C>
bool
is_empty(const Tree<T, C> &) noexcept;

//=====================================
template <typename T, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, C> &, K &&) noexcept;

//=====================================
template <typename T, typename C, typename... Arg>
std::tuple<T *, bool>
emplace(Tree<T, C> &, Arg &&...) noexcept;

//=====================================
template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &, const K &) noexcept;

//=====================================
//=====================================
template <typename T, typename C>
void
dump(Tree<T, C> &tree, std::string prefix = "") noexcept;

//=====================================
template <typename T, typename C>
bool
verify(Tree<T, C> &tree) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
template <typename K>
Node<T>::Node(K &&k, Node<T> *p) noexcept
    : left(nullptr)
    , right(nullptr)
    , parent(p)
    , value(std::forward<K>(k)) {
}

template <>
inline Node<int>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::to_string(int(value)));
  s.append("]");
  return s;
}

template <typename T>
Node<T>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::string(value));
  s.append("]");
  return s;
}

template <typename T>
Node<T>::~Node() noexcept {
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

//=====================================
namespace impl {
namespace binary {

//=====================================
template <typename T>
bool
verify(Node<T> *parent, Node<T> *tree) noexcept {
  if (tree) {
    if (!verify(tree, tree->left)) {
      // printf("left-");
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
        // printf("left-");
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
//=====================================
template <typename T, typename C>
bool
is_empty(const Tree<T, C> &self) noexcept {
  return self.root == nullptr;
}

//=====================================
template <typename T, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, C> &tree, K &&value) noexcept {
  auto result = bst::impl::insert(tree, std::forward<K>(value));
  Node<T> *node = std::get<0>(result);
  bool status = std::get<1>(result);
  T *val = nullptr;
  if (node) {
    val = &node->value;
  }
  return std::make_tuple(val, status);
}

//=====================================
template <typename T, typename C, typename... Arg>
std::tuple<T *, bool>
emplace(Tree<T, C> &self, Arg &&... args) noexcept {
  // TODO make actual impl
  return insert(self, T(std::forward<Arg>(args)...));
}

//=====================================
template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &tree, const K &k) noexcept {
  auto set_root = [&tree](Node<T> *root) {
    if (root) {
      if (root->parent == nullptr) {
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

//=====================================
template <typename T, typename C>
void
dump(Tree<T, C> &tree, std::string prefix) noexcept {
  return bst::impl::dump(tree.root, prefix);
} // binary::dump()

//=====================================
template <typename T, typename C>
bool
verify(Tree<T, C> &tree) noexcept {
  return impl::binary::verify((Node<T> *)nullptr, tree.root);
} // binary::verify()

} // namespace binary

#endif
