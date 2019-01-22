#ifndef SP_UTIL_TREE_AVL_TREE_H
#define SP_UTIL_TREE_AVL_TREE_H

#include <cstdint>
#include <limits.h>
#include <tree/tree.h>
#include <tuple>
#include <util/assert.h>
#include <util/comparator.h>
#include <utility>

namespace avl {
//=====================================
template <typename T>
struct Node {
  using value_type = T;

  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;
  T value;
  std::size_t height;

  template <typename K>
  explicit Node(K &&, Node<T> * = nullptr) noexcept;

  explicit operator std::string() const;

  ~Node() noexcept;
};

template <typename T, typename Comparator = sp::greater>
using Tree = bst::Tree<Node<T>, Comparator>;

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
const T *
find(const Tree<T, C> &, const K &) noexcept;

template <typename T, typename C, typename K>
T *
find(Tree<T, C> &, const K &) noexcept;

//=====================================
template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &, const K &) noexcept;

//=====================================
template <typename T, typename C>
void
dump(const Tree<T, C> &tree, std::string prefix = "") noexcept;

//=====================================
template <typename T, typename C>
bool
verify(const Tree<T, C> &) noexcept;

//=====================================
template <typename T, typename C>
bool
is_empty(const Tree<T, C> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T>
template <typename K>
Node<T>::Node(K &&v, Node<T> *p) noexcept
    : left(nullptr)
    , right(nullptr)
    , parent(p)
    , value(std::forward<K>(v))
    , height(1) {
}

namespace impl {
template <typename T>
static std::size_t
height(const Node<T> *node) noexcept {
  return node ? node->height : 0;
}

template <typename T>
static ssize_t
balance(const Node<T> *node) noexcept {
  assertx(node);
  std::size_t left = height(node->left);
  std::size_t right = height(node->right);

  assertxs(left < SSIZE_MAX, left, SSIZE_MAX);
  assertxs(right < SSIZE_MAX, right, SSIZE_MAX);

  return -ssize_t(left) + ssize_t(right);
} // avl::impl::balance()
} // namespace impl

template <>
inline Node<int>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::to_string(int(value)));
  s.append("|b:");
  s.append(std::to_string(impl::balance(this)));
  s.append("|h:");
  s.append(std::to_string(height));
  s.append("]");
  return s;
}

template <>
inline Node<char>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.push_back(char(value));
  s.append("|b:");
  s.append(std::to_string(impl::balance(this)));
  s.append("|h:");
  s.append(std::to_string(height));
  s.append("]");
  return s;
}

template <typename T>
Node<T>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::string(value));
  s.append("|b:");
  s.append(std::to_string(impl::balance(this)));
  s.append("|h:");
  s.append(std::to_string(height));
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
template <typename T>
static std::size_t
calc_height(Node<T> *const node) noexcept {
  assertx(node);
  node->height = std::max(height(node->left), height(node->right)) + 1;
  return node->height;
}

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
  Node<T> *const A_parent = A->parent;
  Node<T> *const B = A->right;
  Node<T> *const x1 = B ? B->left : nullptr;

  //#Rotate
  A->parent = B;
  A->right = x1;
  if (x1) {
    x1->parent = A;
  }

  if (B) {
    B->parent = A_parent;
    B->left = A;
  } else {
    A->parent = A_parent;
  }

  calc_height(A);
  if (B) {
    calc_height(B);
  }

  assertx(bst::impl::doubly_linked(A));
  assertx(bst::impl::doubly_linked(B));

  assertx(bst::impl::doubly_linked(x1));

  return B ? B : A;
} // avl::impl::rotate_light()

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
  Node<T> *const C_parent = C->parent;
  Node<T> *const B = C->left;
  Node<T> *const x1 = B ? B->right : nullptr;

  //#Rotate
  C->parent = B;
  C->left = x1;

  if (x1) {
    x1->parent = C;
  }

  if (B) {
    B->parent = C_parent;
    B->right = C;
  }

  calc_height(C);
  calc_height(B);

  assertx(bst::impl::doubly_linked(B));
  assertx(bst::impl::doubly_linked(C));

  assertx(bst::impl::doubly_linked(x1));

  return B;
} // avl::impl::rotate_right()

//=====================================
template <typename T>
static Node<T> *&
set(Node<T> *&child) noexcept {
  assertx(child);

  Node<T> *const parent = child->parent;

  if (!parent) {
    return child;
  }

  if (parent->left == child) {
    assertx(parent->right != child);
    return parent->left;
  }

  assertx(parent->right == child);
  return parent->right;
} // avl::impl::set()

template <typename T>
static Node<T> *
rebalance(Node<T> *root) noexcept {
  assertx(root);

  Node<T> *atleast = root;
  {
    const auto before = atleast->height;
    assertx(before == calc_height(atleast));
  }
  root = root->parent;

  while (root) {
    calc_height(root);

    if (balance(root) == 0) {
      return root;
    }

    /* Left Heavy */
    if (balance(root) == -2) {
      if (balance(root->left) == 1) {
        root->left = rotate_left(root->left);
        assertx(bst::impl::doubly_linked(root->left));
        assertx(bst::impl::doubly_linked(root));
      }

      // update parent with new child
      auto &s = set(root);
      s = rotate_right(root);
      assertx(bst::impl::doubly_linked(root));
      return root;
    }
    /* Right Heavy */
    else if (balance(root) == 2) {
      if (balance(root->right) == -1) {
        root->right = rotate_right(root->right);
        assertx(bst::impl::doubly_linked(root->right));
        assertx(bst::impl::doubly_linked(root));
      }

      auto &s = set(root);
      s = rotate_left(root);

      assertx(bst::impl::doubly_linked(root));
      return root;
    }

    atleast = root;
    root = atleast->parent;
  }

  return atleast;
}

#if 0
template <typename T>
static bool
assert_no_cycle(Node<T> *tree, binary::Tree<Node<T> *> &visited) {
  if (tree) {
    if (find(visited, tree)) {
      return false;
    }

    auto res = insert(visited, tree);
    assertx(std::get<1>(res));

    if (!assert_no_cycle(tree->left, visited)) {
      return false;
    }

    if (!assert_no_cycle(tree->right, visited)) {
      return false;
    }
  }

  return true;
}

template <typename T>
static bool
assert_no_cycle(Node<T> *tree) {
  binary::Tree<Node<T> *> visited;
  return assert_no_cycle(tree, visited);
  // return true;
}
#endif

} // namespace impl

template <typename T, typename C, typename V>
std::tuple<T *, bool>
insert(Tree<T, C> &self, V &&value) noexcept {
  std::tuple<Node<T> *, bool> result =
      bst::impl::insert(self, std::forward<V>(value));

  Node<T> *const node = std::get<0>(result);
  const bool inserted{std::get<1>(result)};
  if (inserted) {
    assertx(node);

    Node<T> *root = impl::rebalance(node);
    assertx(root);

    // TODO fix
    // while (root->parent) {
    //   root = root->parent;
    // }
    // self.root = root;
    if (root->parent == nullptr) {
      self.root = root;
    }

    // assertx(impl::assert_no_cycle(self.root));
    assertx(self.root->parent == nullptr);
  }

  T *insval = nullptr;
  if (node) {
    insval = &node->value;
  }

  return std::make_tuple(insval, inserted);
}

//=====================================
template <typename T, typename C, typename... Arg>
std::tuple<T *, bool>
emplace(Tree<T, C> &self, Arg &&... args) noexcept {
  // TODO make correct
  return insert(self, T(std::forward<Arg>(args)...));
} // avl::emplace()

//=====================================
template <typename T, typename C, typename K>
const T *
find(const Tree<T, C> &self, const K &needle) noexcept {
  return bst::find(self, needle);
} // avl::find()

template <typename T, typename C, typename K>
T *
find(Tree<T, C> &self, const K &needle) noexcept {
  return bst::find(self, needle);
} // avl::find()

//=====================================
namespace impl {

template <typename T>
static Node<T> *
find_min(Node<T> *node) noexcept {
  assertx(node);
  while (node->left) {
    node = node->left;
  }
  return node;
}

template <typename T>
static Node<T> *
reb(Node<T> *root) noexcept {
  auto atleast = root;
  while (root) {
    calc_height(root);

    // if (balance(root) == 0) {
    // }

    /* Left Heavy */
    if (balance(root) == -2) {
      if (balance(root->left) == 1) {
        root->left = rotate_left(root->left);
        assertx(bst::impl::doubly_linked(root->left));
        assertx(bst::impl::doubly_linked(root));
      }

      // update parent with new child
      auto &s = set(root);
      s = rotate_right(root);
      assertx(bst::impl::doubly_linked(root));
    }
    /* Right Heavy */
    else if (balance(root) == 2) {
      if (balance(root->right) == -1) {
        root->right = rotate_right(root->right);
        assertx(bst::impl::doubly_linked(root->right));
        assertx(bst::impl::doubly_linked(root));
      }

      auto &s = set(root);
      s = rotate_left(root);

      assertx(bst::impl::doubly_linked(root));
    }

    atleast = root;
    root = atleast->parent;
  }

  assertx(atleast);
  return atleast;
}

template <typename T, typename C>
static void
take(Tree<T, C> &, Node<T> *) noexcept;

template <typename T, typename C>
static Node<T> *
unlink(Tree<T, C> &self, Node<T> *node) noexcept {
  auto xxx = [](Node<T> *parent, Node<T> *priv, Node<T> *subject) {
    if (parent) {
      if (parent->left == priv) {
        assertx(parent->right != priv);
        parent->left = subject;
      } else {
        assertx(parent->right == priv);
        parent->right = subject;
      }
    }
    if (subject)
      subject->parent = parent;
  };

  assertx(node);
  if (node->left && node->right) {
    auto hier = find_min(node->right);
    assertx(hier);
    take(self, hier);

    xxx(node->parent, node, hier);
    hier->left = node->left;
    if (hier->left) {
      hier->left->parent = hier;
    }

    hier->right = node->right;
    if (hier->right) {
      hier->right->parent = hier;
    }

    calc_height(hier);

    return reb(hier);
  } else if (node->left) {
    auto hier = node->left;
    xxx(node->parent, node, hier);
    return reb(hier);
  } else if (node->right) {
    auto hier = node->right;
    xxx(node->parent, node, hier);
    return reb(hier);
  }
  // assertx(false);

  auto parent = node->parent;
  xxx(parent, node, nullptr);
  if (parent) {
    return reb(parent);
  }

  return nullptr;
}

template <typename T, typename C>
static void
take(Tree<T, C> &self, Node<T> *node) noexcept {
  Node<T> *const root = unlink(self, node);
  if (root) {
    if (!root->parent) {
      self.root = root;
    }
    assertx(self.root->parent == nullptr);
    // if (!verify(self)) {
    //   // printf("\nunlink(%s) before:\n", std::string(*node).c_str());
    //   printf("take-failed:\n");
    //   dump(self);
    //   assertx(verify(self));
    // }
  } else {
    self.root = nullptr;
  }

  {
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
  }
}

} // namespace impl

template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &self, const K &needle) noexcept {
  Node<T> *const node = bst::impl::find_node(self, needle);
  if (node) {
    impl::take(self, node);

    assertx(!node->left);
    assertx(!node->right);
    delete node;

    return true;
  }

  return false;
} // avl::remove()

//=====================================
template <typename T, typename C>
void
dump(const Tree<T, C> &tree, std::string prefix) noexcept {
  return bst::impl::dump(tree.root, prefix);
} // avl::dump()

//=====================================
namespace impl {
template <typename T>
static bool
verify(const Node<T> *parent, const Node<T> *tree, ssize_t &result) noexcept {
  result = 0;
  if (tree) {
    if (tree->parent != parent) {
      return false;
    }

    ssize_t left = 0;
    if (tree->left) {
      if (!(tree->value > tree->left->value)) {
        return false;
      }
      if (!verify(tree, tree->left, left)) {
        return false;
      }
    }

    ssize_t right = 0;
    if (tree->right) {
      if (!(tree->value < tree->right->value)) {
        return false;
      }
      if (!verify(tree, tree->right, right)) {
        return false;
      }
    }

    result++;

    ssize_t bl = ssize_t(right) - ssize_t(left);
    const auto h = std::max(right, left) + 1;
    assertx(h >= 0);
    if (std::size_t(h) != tree->height) {
      // printf("height[%d] != tree->height[%d]\n", h, tree->height);
      return false;
    }

    if (balance(tree) != bl) {
      /*
       * std::cout << "right: " << right << "|";
       * std::cout << "left: " << left << "|";
       * // std::cout << "bl: " << bl << "|";
       * std::cout << "bl: " << int(bl) << "|";
       * std::cout << "tree: " << std::string(*tree) << "|";
       * std::cout << "\n";
       */
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

template <typename T, typename C>
bool
verify(const Tree<T, C> &self) noexcept {
  ssize_t balance = 0;
  return impl::verify<T>((Node<T> *)nullptr, self.root, balance);
} // avl::verify()

//=====================================
template <typename T, typename C>
bool
is_empty(const Tree<T, C> &self) noexcept {
  return self.root == nullptr;
}

//=====================================
} // namespace avl

#endif
