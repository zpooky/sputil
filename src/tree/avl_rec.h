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
  std::size_t height;

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
bool
is_empty(const Tree<T> &) noexcept;

//=====================================
template <typename T>
void
dump(const Tree<T> &) noexcept;

//=====================================
template <typename T>
bool
verify(const Tree<T> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
template <typename V>
Node<T>::Node(V &&val) noexcept
    : value(std::forward<V>(val))
    , left{nullptr}
    , right{nullptr}
    , height{1} {
}

template <typename T>
Node<T>::~Node() noexcept {
  if (left) {
    delete left;
  }
  left = nullptr;
  if (right) {
    delete right;
  }
  right = nullptr;
}

//=====================================
template <typename T>
Tree<T>::Tree() noexcept
    : root{nullptr} {
}

template <typename T>
Tree<T>::~Tree() noexcept {
  if (root) {
    delete root;
  }
  root = nullptr;
}

namespace impl {
//=====================================
template <typename T>
static std::size_t
height(const Node<T> *node) noexcept {
  return node ? node->height : 0;
}

template <typename T>
static int
balance(const Node<T> *node) noexcept {
  assertx(node);
  return -height(node->left) + height(node->right);
} // avl::rec::impl::balance()

//=====================================
template <typename T>
static std::size_t
calc_height(Node<T> *node) noexcept {
  assertx(node);
  node->height = std::max(height(node->left), height(node->right)) + 1;
  return node->height;
}

//=====================================
template <typename T>
static Node<T> *
rotate_left(Node<T> *const A) noexcept {
  // printf("rotate_left([%c|b:%d|h:%zu])\n", char(A->value), balance(A),
  //        height(A));
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

  calc_height(A);
  if (B) {
    calc_height(B);
  }

  return B ? B : A;
} // avl::rec::impl::rotate_light()

//=====================================
template <typename T>
static Node<T> *
rotate_right(Node<T> *const C) noexcept {
  // printf("rotate_right([%c|b:%d|h:%zu])\n", char(C->value), balance(C),
  //        height(C));
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

  calc_height(C);
  calc_height(B);

  return B;
} // avl::rec::impl::rotate_right()

//=====================================
template <typename T>
static Node<T> *
rebalance(Node<T> *const root, bool &do_balance) noexcept {
  assertx(root);

  if (!do_balance) {
    return root;
  }

  calc_height(root);

  if (balance(root) == 0) {
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

      return rebalance(root, balance);
    } else if (value > val) {
      root->left = insert(root->left, std::forward<V>(val), out, balance);

      return rebalance(root, balance);
    }
    // equal
    out = &root->value;
    balance = false;

    return root;
  }

  out = nullptr;
  balance = false;

  auto result = new (std::nothrow) Node<T>(std::forward<V>(val));
  if (result) {
    out = &result->value;
    balance = true;
  }

  return result;
}

} // namespace impl

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
  /*tailrec function*/
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

template <typename T, typename V>
static Node<T> *
find(Node<T> *root, const V &needle) noexcept {
  const Node<T> *const c_root = root;
  return (Node<T> *)find(c_root, needle);
}
} // namespace impl

template <typename T, typename V>
T *
find(Tree<T> &tree, const V &needle) noexcept {
  Node<T> *const result = impl::find(tree.root, needle);
  if (result) {
    return &result->value;
  }

  return nullptr;
}

//=====================================
namespace impl {
template <typename T>
static Node<T> *
find_min(Node<T> *root) noexcept {
  /*tailrec function*/
  assertx(root);
  if (root->left) {
    return find_min(root->left);
  }

  return root;
}

template <typename T, typename V>
static Node<T> *
unlink(Node<T> *, const V &, Node<T> *&, bool &) noexcept;

template <typename T>
static Node<T> *
do_unlink(Node<T> *const root, bool &balance) noexcept {
  assertx(root);

  Node<T> *result = nullptr;
  if (root->left && root->right) {
    Node<T> *const heir = find_min(root->right);
    assertx(heir);

    {
      Node<T> *out_min = nullptr;
      // TODO maybe unlink /hier/Node instead of hier->value
      heir->right = unlink(root->right, heir->value, out_min, balance);
      assertx(out_min);
      assertx(heir == out_min);
    }

    heir->left = root->left;
    result = heir;
  } else {
    if (root->left) {
      result = root->left;
    } else if (root->right) {
      result = root->right;
    }
    balance = true;
  }

  root->left = nullptr;
  root->right = nullptr;

  return result;
}

template <typename T, typename V>
static Node<T> *
unlink(Node<T> *root, const V &needle, Node<T> *&out, bool &balance) noexcept {
  if (root) {
    const T &value = root->value;
    if (needle > value) {
      root->right = unlink(root->right, needle, out, balance);
    } else if (value > needle) {
      root->left = unlink(root->left, needle, out, balance);
    } else {
      out = root;
      balance = true;
      root = do_unlink(root, balance);
      if (root == nullptr)
        return root;
    }

    /* on remove() we should not stop after first rebalance as we do for
     * insert()
     */
    bool before = balance;
    return rebalance(root, before);
  }

  out = nullptr;
  balance = false;
  return nullptr;
}
} // namespace impl

template <typename T, typename V>
bool
remove(Tree<T> &self, const V &needle) noexcept {
  Node<T> *result = nullptr;
  bool balance = false;
  self.root = impl::unlink(self.root, needle, result, balance);

  if (result) {
    delete result;
    return true;
  }

  return false;
}

//=====================================
template <typename T>
bool
is_empty(const Tree<T> &self) noexcept {
  return self.root == nullptr;
}

//=====================================
namespace impl {
template <std::size_t n>
static void
format_node(const Node<char> *tree, const char *ctx, char (&name)[n]) noexcept {
  sprintf(name, "%s[%c|b:%d|h:%zu]", ctx, tree->value, balance(tree),
          height(tree));
}

template <std::size_t n>
static void
format_node(const Node<int> *tree, const char *ctx, char (&name)[n]) noexcept {
  sprintf(name, "%s[%d|b:%d|h:%zu]", ctx, tree->value, balance(tree),
          height(tree));
}

template <typename T>
static void
dump(Node<T> *tree, std::string prefix = "", bool isTail = true,
     const char *ctx = "") noexcept {
  if (tree) {
    char name[256] = {0};
    format_node(tree, ctx, name);

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

} // namespace impl

template <typename T>
void
dump(const Tree<T> &self) noexcept {
  return impl::dump(self.root);
}

//=====================================
namespace impl {
template <typename T>
static bool
verify(const Node<T> *tree, int &result) noexcept {
  result = 0;
  if (tree) {
    int left = 0;
    if (tree->left) {
      if (!(tree->value > tree->left->value)) {
        return false;
      }
      if (!verify(tree->left, left)) {
        return false;
      }
    }

    int right = 0;
    if (tree->right) {
      if (!(tree->value < tree->right->value)) {
        return false;
      }
      if (!verify(tree->right, right)) {
        return false;
      }
    }

    result++;

    int bl = int(right) - int(left);
    const int h = std::max(right, left) + 1;
    if (h != tree->height) {
      // printf("height[%d] != tree->height[%d]\n", h, tree->height);
      return false;
    }

    if (balance(tree) != bl) {
      std::cout << "right: " << right << "|";
      std::cout << "left: " << left << "|";
      // std::cout << "bl: " << bl << "|";
      std::cout << "bl: " << int(bl) << "|";
      // std::cout << "tree: " << std::string(*tree) << "|";
      std::cout << "\n";
      return false;
    }

    if (balance(tree) > 1) {
      return false;
    }
    if (balance(tree) < -1) {
      return false;
    }

    result += std::max(left, right);
  }
  return true;
} // avl::impl::verify()
} // namespace impl

template <typename T>
bool
verify(const Tree<T> &self) noexcept {
  int balance = 0;
  return impl::verify<T>(self.root, balance);
} // avl::verify()

//=====================================
} // namespace rec
} // namespace avl

#endif
