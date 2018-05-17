#ifndef SP_UTIL_TREE_AVL_TREE_REC_H
#define SP_UTIL_TREE_AVL_TREE_REC_H

#include <memory>
#include <string> //dump
#include <util/assert.h>
#include <utility>

namespace avl {
namespace rec {
//=====================================
template <typename T>
struct Node {
  T value;
  Node<T> *left;
  Node<T> *right;
  int balance;

  template <typename V>
  explicit Node(V &&) noexcept;

  ~Node() noexcept;
};

//=====================================
template <typename T>
struct Tree {
  Node<T> *root;

  Tree() noexcept;

  ~Tree() noexcept;
};

//=====================================
template <typename T, typename V>
T *
insert(Tree<T> &, V &&) noexcept;

//=====================================
template <typename T, typename V>
T *
find(Tree<T> &, const V &) noexcept;

//=====================================
template <typename T, typename V>
bool
remove(Tree<T> &, const V &) noexcept;

//=====================================
template <typename T>
void
dump(const Tree<T> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
template <typename V>
Node<T>::Node(V &&val) noexcept
    : value(std::forward<V>(val))
    , left{nullptr}
    , right{nullptr}
    , balance{0} {
}

template <typename T>
Node<T>::~Node() noexcept {
  // TODO
}

//=====================================
template <typename T>
Tree<T>::Tree() noexcept
    : root{nullptr} {
}

template <typename T>
Tree<T>::~Tree() noexcept {
  // TODO
}

namespace impl {
//=====================================
template <typename T>
static int
balance(const Node<T> *node) noexcept {
  return node ? node->balance : 0;
} // avl::rec::impl::balance()

//=====================================
template <typename T>
static Node<T> *
rotate_left(Node<T> *const A) noexcept {
  // printf("\trotate_left(%s)\n", std::string(*A).c_str());
  /*
   * <_
   *   \
   *B__/
   *
   * C:3, B:2, A:1
   *
   * A                           B
   *  \           L(1)          / \
   *   B          ---->        A   C
   *  / \                       \
   * x1  C                       x1
   */
  // B is optional therefore it can be null
  Node<T> *const B = A->right;
  Node<T> *const x1 = B ? B->left : nullptr;
  if (B) {
    B->left = A;
    A->right = x1;
  }

  // #if 0
  //#Update Balance
  /*We do not rebalance C since its children has not been altered*/

  // TODO this needs to be documented
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
  // #endif

  return B ? B : A;
} // avl::rec::impl::rotate_light()

//=====================================
template <typename T>
static Node<T> *
rotate_right(Node<T> *const C) noexcept {
  // printf("\trotate_right(%s)\n", std::string(*C).c_str());
  /*
   *B_.
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
  // B is optional therefore it can be null
  Node<T> *const B = C->left;
  Node<T> *const x1 = B ? B->right : nullptr;
  B->right = C;
  C->left = x1;

  // #if 0
  // TODO this needs to be documented
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

  // #endif
  return B;
} // avl::rec::impl::rotate_right()

//=====================================
template <typename T>
static Node<T> *
rebalance(Node<T> *const root, bool &do_balance) noexcept {
  assertx(root);

  if (do_balance == false) {
    return root;
  }

  if (root->balance == 0) {
    do_balance = false;
    return root;
  }

  /* Left Heavy */
  if (balance(root) == -2) {
    if (balance(root->left) == 1) {
      root->left = rotate_left(root->left);
    }

    do_balance = false;
    // update parent with new child
    return rotate_right(root);
  }
  /* Right Heavy */
  else if (balance(root) == 2) {
    if (balance(root->right) == -1) {
      root->right = rotate_right(root->right);
    }

    do_balance = false;
    return rotate_left(root);
  }

  return root;
}

//=====================================
template <typename T, typename V>
static Node<T> *
insert(Node<T> *root, V &&val, T *&out, bool &balance) noexcept {
  if (root) {
    const T &value = root->value;
    if (val > value) {
      root->right = insert(root->right, std::forward<V>(val), out, balance);
      if (balance) {
        ++root->balance;
      }

      return rebalance(root, balance);
    } else if (value > val) {
      root->left = insert(root->left, std::forward<V>(val), out, balance);
      if (balance) {
        --root->balance;
      }

      return rebalance(root, balance);
    }
    // equal
    out = &root->value;
    balance = false;

    return root;
  }

  auto result = new (std::nothrow) Node<T>(std::forward<V>(val));
  if (result) {
    out = &result->value;
    balance = true;
  }
  return result;
}

} // namespace avl::rec::impl

template <typename T, typename V>
T *
insert(Tree<T> &self, V &&val) noexcept {
  bool rebalance = true;
  T *result = nullptr;
  self.root = impl::insert(self.root, std::forward<V>(val), result, rebalance);
  return result;
}

//=====================================
namespace impl {
template <typename T, typename V>
static const Node<T> *
find(const Node<T> *root, const V &needle) noexcept {
  if (root) {
    const T &value = root->value;
    if (needle > value) {
      return find(root->right, needle);
    } else if (value > needle) {
      return find(root->left, needle);
    } else {
      // equal
      return root;
    }
  }

  return nullptr;
}
}

template <typename T, typename V>
T *
find(Tree<T> &tree, const V &needle) noexcept {
  const Node<T> *result = impl::find(tree.root, needle);
  if (result) {
    return (T *)&result->value;
  }

  return nullptr;
}

//=====================================
template <typename T, typename V>
bool
remove(Tree<T> &, const V &) noexcept {
  return false;
}

//=====================================
namespace impl {
inline void
dump(Node<char> *tree, std::string prefix = "", bool isTail = true,
     const char *ctx = "") noexcept {
  if (tree) {
    char name[256] = {0};
    sprintf(name, "%s[%c|b:%d]", ctx, tree->value, tree->balance);

    printf("%s%s%s\n", prefix.c_str(), (isTail ? "└──" : "├──"), name);
    const char *ls = (isTail ? "   " : "│  ");
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
} // namespace avl::rec::impl

template <typename T>
void
dump(const Tree<T> &self) noexcept {
  return impl::dump(self.root);
}

//=====================================
} // namespace avl::rec
} // namespace avl

#endif
