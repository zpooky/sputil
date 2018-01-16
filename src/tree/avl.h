#ifndef SP_UTIL_TREE_AVL_TREE_H
#define SP_UTIL_TREE_AVL_TREE_H

#include "tree.h"
#include <cassert>
#include <cstdint>
#include <tuple>
#include <utility>

#include <iostream>
#include <string> //debug

// TODO noexcept operator

namespace avl {
  /*avl*/

template <typename T>
struct Node {
  using value_type = T;

  // TODO lesser
  Node<T> *left;
  // TODO greater
  Node<T> *right;
  Node<T> *parent;
  T value;
  std::int8_t balance;

  template <typename K>
  explicit Node(K &&v, Node<T> *p = nullptr)
      : left(nullptr)
      , right(nullptr)
      , parent(p)
      , value(std::forward<K>(v))
      , balance(0) {
  }

  explicit operator std::string() const {
    std::string s;
    s.append("[v:");
    s.append(std::to_string(int(value)));
    s.append("|b:");
    s.append(std::to_string(balance));
    s.append("]");
    return s;
  }

  template <typename O>
  bool
  operator<(const O &o) const noexcept {
    return value < o;
  }

  template <typename O>
  bool
  operator>(const O &o) const noexcept {
    return value > o;
  }

  template <typename O>
  bool
  operator==(const O &o) const noexcept {
    return value == o;
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

template <typename T>
using Tree = sp::Tree<avl::Node<T>>;

template <typename T>
void
dump(Tree<T> &tree, std::string prefix = "") noexcept;

template <typename T>
bool
verify(Tree<T> &tree) noexcept;

template <typename T,typename K>
const T* find(const Tree<T>&,const K&) noexcept;


namespace impl {
  /*avl::impl*/
namespace avl {
  /*avl::impl::avl*/
// BalanceFactor(N) := Height(RightSubtree(N)) - Height(LeftSubtree(N))
// BalanceFactor(N) = {–1, 0, +1}
//
// BalanceFactor holds for every node N in the tree.
// A node N with BalanceFactor(N) < 0 is called "left-heavy", one with
// BalanceFactor(N) > 0 is called "right-heavy", and one with BalanceFactor(N)
// = 0 is sometimes simply called "balanced".

enum class Direction : bool { LEFT, RIGHT };

template <typename T>
static Direction
direction(const Node<T> *const child) noexcept {
  assert(child);
  Node<T> *const parent = child->parent;
  assert(parent);

  if (parent->left == child) {
    return Direction::LEFT;
  }

  assert(parent->right == child);
  return Direction::RIGHT;
}// avl::impl::avl::direction()

template <typename T>
void
dump_root(Node<T> *tree, std::string prefix = "") noexcept {
  Node<T> *root = tree;
Lstart:
  if (root->parent) {
    root = root->parent;
    goto Lstart;
  }
  return sp::impl::tree::dump(root, prefix);
}// avl::impl::avl::dump_root()

template <typename T>
static std::int8_t
balance(const Node<T> *const node) noexcept {
  return node ? node->balance : 0;
}// avl::impl::avl::balance()

template <typename T>
static Node<T> *
rotate_left(Node<T> *const A) noexcept {
  // printf("\trotate_left(%s)\n", std::string(*A).c_str());
  // dump_root(A, "\t");
  /*
   * <_
   *   \
   * __/
   *
   * C:3, B:2, A:1
   *
   * A                           B
   *  \           L(1)          / \
   *   B          ---->        A   C
   *  / \                       \
   * x1  C                       x1
   */
  Node<T> *const A_parent = A->parent;
  Node<T> *const B = A->right;
  Node<T> *const B_left = B ? B->left : nullptr;

  //#Rotate
  A->parent = B;
  A->right = B_left;

  if (B_left) {
    B_left->parent = A;
  }

  if (B) {
    B->parent = A_parent;
    B->left = A;
  }

  //#Update Balance
  /*We do not rebalance C since its children has not been altered*/

  A->balance -= 1;
  if (balance(B) > 0) {
    A->balance -= B->balance;
  }
  if (B) {
    B->balance -= 1;
    if (balance(A) < 0) {
      B->balance -= -A->balance;
    }
  }

  assert(sp::impl::tree::doubly_linked(A));
  assert(sp::impl::tree::doubly_linked(B));

  // assert(sp::impl::tree::doubly_linked(A_parent));
  assert(sp::impl::tree::doubly_linked(B_left));

  return B ? B : A;
}// avl::impl::avl::rotate_light()

template <typename T>
static Node<T> *
rotate_right(Node<T> *const C) noexcept {
  // printf("\trotate_right(%s)\n", std::string(*C).c_str());
  // dump_root(C, "\t");
  /*
  * _.
  *   \
  * <-´
  *
  * C:3, B:2, A:1
  *
  *     C                         B
  *    /           R(3)          / \
  *   B            ---->        A   C
  *  / \                           /
  * A   x1                        x1
  */
  Node<T> *const C_parent = C->parent;
  Node<T> *const B = C->left;
  Node<T> *const B_right = B ? B->right : nullptr;

  //#Rotate
  C->parent = B;
  C->left = B_right;

  if (B_right) {
    B_right->parent = C;
  }

  if (B) {
    B->parent = C_parent;
    B->right = C;
  }

  // x = C
  // y = B
  C->balance += 1;
  if (balance(B) < 0) {
    C->balance += -B->balance;
  }

  if (B) {
    B->balance += 1;
    if (balance(C) > 0) {
      B->balance += C->balance;
    }
  }

  assert(sp::impl::tree::doubly_linked(B));
  assert(sp::impl::tree::doubly_linked(C));

  assert(sp::impl::tree::doubly_linked(B_right));
  // assert(sp::impl::tree::doubly_linked(C_parent));

  return B;
}// avl::impl::avl::rotate_right()

template <typename T>
static Node<T> *&
set(Node<T> *&child) noexcept {
  assert(child);

  Node<T> *const parent = child->parent;

  if (!parent) {
    return child;
  }

  if (parent->left == child) {
    return parent->left;
  }

  assert(parent->right == child);
  return parent->right;
}// avl::impl::avl::set()

template <typename T>
bool
verify(const Node<T> *parent, const Node<T> *tree,
       std::uint32_t &result) noexcept {
  result = 0;
  if (tree) {
    if (tree->parent != parent) {
      return false;
    }

    std::uint32_t left = 0;
    if (tree->left) {
      if (!(tree->value > tree->left->value)) {
        return false;
      }
      if (!verify(tree, tree->left, left)) {
        return false;
      }
    }

    std::uint32_t right = 0;
    if (tree->right) {
      if (!(tree->value < tree->right->value)) {
        return false;
      }
      if (!verify(tree, tree->right, right)) {
        return false;
      }
    }

    result++;

    std::int64_t bl = std::int64_t(right) - std::int64_t(left);
    std::int8_t b = bl;
    if (tree->balance != b) {
      std::cout << "right: " << right << "|";
      std::cout << "left: " << left << "|";
      // std::cout << "bl: " << bl << "|";
      std::cout << "b: " << int(b) << "|";
      std::cout << "tree: " << std::string(*tree) << "|";
      std::cout << "\n";
    }

    assert(bl == b);
    if (tree->balance != b) {
      return false;
    }

    if ((tree->balance > 1)) {
      return false;
    }
    if ((tree->balance < -1)) {
      return false;
    }

    result += std::max(left, right);
  }
  return true;
} // avl::impl::avl::verify()

template <typename T>
void
exchange(Node<T> *node, Node<T> *n) noexcept {
  auto *parent = node->parent;
  if (parent) {
    if (parent->left == node) {
      parent->left = n;
    }
    if (parent->right != node) {
      assert(parent->right == node);
    }
    parent->right = n;
  }

  if (n) {
    n->parent = parent;

    n->left = node->left;
    if (n->left) {
      n->left->parent = n;
    }

    n->right = node->right;
    if (n->right) {
      n->right->parent = n;
    }
  }
} // avl::impl::avl::exchange()

template <typename T>
static bool
verify(Node<T> *const tree) {
  if (!tree) {
    return true;
  }

  std::uint32_t balance = 0;
  return verify(tree->parent, tree, balance);
}//avl::impl::avl::verify()

template <typename T>
static bool
verify_root(Node<T> *const tree) {
  Node<T> *root = tree;
Lstart:
  if (root->parent) {
    root = root->parent;
    goto Lstart;
  }

  std::uint32_t balance = 0;
  return verify((Node<T> *)nullptr, root, balance);
}//avl::impl::avl::verify_root()

} // namespace impl
} // namespace avl

template <typename T>
void
dump(Tree<T> &tree, std::string prefix) noexcept {
  return sp::impl::tree::dump(tree.root, prefix);
}//avl::dump()

template <typename T>
bool
verify(Tree<T> &tree) noexcept {
  std::uint32_t balance = 0;
  return impl::avl::verify((Node<T> *)nullptr, tree.root, balance);
}//avl::verify()

template <typename T,typename K>
const T* find(const Tree<T>&tree,const K&key) noexcept {
  return sp::find(tree,key);
}//av::find()



} // namespace avl

#endif
