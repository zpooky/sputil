#ifndef SP_UTIL_TREE_RB_TREE_H
#define SP_UTIL_TREE_RB_TREE_H

#include <tree/tree.h>
#include <tuple>
#include <utility> //forward

namespace rb {

//=====================================
enum class Colour : bool { RED, BLACK };

template <typename T>
struct Node {
  using value_type = T;

  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;
  T value;
  Colour colour;

  template <typename K>
  explicit Node(K &&v, Node<T> *p = nullptr) noexcept;

  explicit operator std::string() const;
  ~Node() noexcept;
};

template <typename T, typename Comparator = sp::greater>
using Tree = bst::Tree<rb::Node<T>, Comparator>;

//=====================================
template <typename T, typename C, typename K>
const T *
find(const Tree<T, C> &, const K &) noexcept;

template <typename T, typename C, typename K>
T *
find(Tree<T, C> &, const K &) noexcept;

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
template <typename T, typename C>
void
dump(Tree<T, C> &tree, const std::string &prefix = "") noexcept;

//=====================================
template <typename T, typename C>
bool
verify(Tree<T, C> &tree) noexcept;

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
    , colour(Colour::RED) {
}

template <>
inline Node<int>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::to_string(int(value)));
  s.append("|b:");
  s.append(colour == Colour::RED ? "RED" : "");
  s.append(colour == Colour::BLACK ? "BLACK" : "");
  s.append("]");
  return s;
}

template <typename T>
Node<T>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::string(value));
  s.append("|b:");
  s.append(colour == Colour::RED ? "RED" : "");
  s.append(colour == Colour::BLACK ? "BLACK" : "");
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
namespace rb {

//=====================================
template <typename T>
static Node<T> *
parent(Node<T> *n) {
  assertx(n);
  return n->parent;
}

template <typename T>
static Node<T> *
grandparent(Node<T> *n) {
  Node<T> *p = parent(n);
  assertx(bst::impl::doubly_linked(p));
  return p ? parent(p) : nullptr;
}

template <typename T>
static Node<T> *
uncle(Node<T> *n) {
  Node<T> *p = parent(n);
  Node<T> *g = grandparent(n);
  if (g->left != p) {
    assertx(g->right == p);
    return g->left;
  }

  assertx(g->left == p);
  return g->right;
}

//=====================================
template <typename T>
static void
rotate_left(Node<T> *const A) noexcept {
  // printf("rotate_left(%d)\n",A->value);
  /*
   *  <_
   *    \
   * B__/
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
  Node<T> *const B_left = B ? B->left : nullptr;
  std::size_t c_before = bst::impl::child_count(A_parent);

  Colour A_colour = A->colour;

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

  assertx(bst::impl::doubly_linked(A));
  assertx(bst::impl::doubly_linked(B));

  assertx(bst::impl::doubly_linked(B_left));

  Node<T> *new_root = B ? B : A;
  if (A_parent) {
    assertx(A_parent->left == A || A_parent->right == A);
    if (A_parent->left == A) {
      A_parent->left = new_root;
    } else {
      A_parent->right = new_root;
    }
  }
  assertx(bst::impl::doubly_linked(A));
  assertx(bst::impl::doubly_linked(B));

  assertx(bst::impl::doubly_linked(B_left));
  assertx(bst::impl::doubly_linked(A_parent));

  new_root->colour = A_colour;
  A->colour = Colour::RED;

  assertx(c_before == bst::impl::child_count(A_parent));
}

//=====================================
template <typename T>
static void
rotate_right(Node<T> *C) noexcept {
  // printf("rotate_right(%d)\n",C->value);
  /*
   * B_.
   *    \
   *  <-´
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
  std::size_t c_before = bst::impl::child_count(C_parent);

  Colour C_colour = C->colour;
  // Colour B_colour = B ? B->colour : Colour::BLACK;
  // Colour A_colour = A->colour;

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

  assertx(bst::impl::doubly_linked(B));
  assertx(bst::impl::doubly_linked(C));

  assertx(bst::impl::doubly_linked(B_right));

  if (C_parent) {
    assertx(C_parent->left == C || C_parent->right == C);

    if (C_parent->left == C) {
      C_parent->left = B;
    } else {
      C_parent->right = B;
    }
  }
  assertx(bst::impl::doubly_linked(B));
  assertx(bst::impl::doubly_linked(C));

  assertx(bst::impl::doubly_linked(B_right));
  assertx(bst::impl::doubly_linked(C_parent));

  B->colour = C_colour;
  C->colour = Colour::RED;
  // A->color = B->colour;

  assertx(c_before == bst::impl::child_count(C_parent));
}

//=====================================
template <typename T>
static Node<T> *
rebalance(Node<T> *n) {
  // printf("rebalance(%d)\n",n->value);
  assertx(n->colour == Colour::RED);

  Node<T> *p = parent(n);
  if (!p) {
    n->colour = Colour::BLACK;
    return n;
  }

  if (p->colour == Colour::BLACK) {
    return p;
  } else if (p->colour == Colour::RED) {
    Node<T> *u = uncle(n);

    if (u && u->colour == Colour::RED) {
      Node<T> *g = grandparent(n);
      assertx(g);

      p->colour = Colour::BLACK;
      u->colour = Colour::BLACK;
      g->colour = Colour::RED;

      return rebalance(g);
    } else {
      {
        Node<T> *g = grandparent(n);
        if (g) {
          Node<T> *p = parent(n);
          // assertx(g->left);
          if (g->left && n == g->left->right) {
            assertx(p);

            rotate_left(p);
            n = n->left;
          } else {
            // assertx(g->right);
            if (g->right && n == g->right->left) {
              assertx(p);

              rotate_right(p);
              n = n->right;
            }
          }
        }
      }

      Node<T> *p = parent(n);
      Node<T> *g = grandparent(n);
      assertx(p);
      if (n == p->left) {
        rotate_right(g);
      } else {
        rotate_left(g);
      }

      p->colour = Colour::BLACK;
      if (g) {
        g->colour = Colour::RED;
        return rebalance(g); // sp
      }
      return p;
    }
  }
  assertx(false);
} // rb::impl::rb::rebalance()

//=====================================
template <typename T>
static bool
verify(Node<T> *parent, Node<T> *current, std::size_t &min, std::size_t &max) {
  if (parent) {
    // The children of a RED coloured node must be coloured BLACK
    if (parent->colour == Colour::RED) {
      if (current->colour != Colour::BLACK) {
        return false;
      }
    }

    if (current->parent != parent) {
      return false;
    }
  } else {
    // The ROOT node must coloured BLACK
    if (current->colour != Colour::BLACK) {
      return false;
    }
  }

  std::size_t l_min, l_max;
  std::size_t r_min, r_max;
  l_min = l_max = 0;
  r_min = r_max = 0;

  if (current->left) {
    if (!(current->left->value < current->value)) {
      return false;
    }
    if (!verify(current, current->left, l_min, l_max)) {
      return false;
    }
  }
  if (current->right) {
    if (!(current->right->value > current->value)) {
      return false;
    }
    if (!verify(current, current->right, r_min, r_max)) {
      return false;
    }
  }

  min = std::min(l_min, r_min) + 1;
  max = std::max(l_max, r_max) + 1;

  // See if this node is balanced
  if (max <= 2 * min) {
    return true;
  }

  return false;
} // rb::impl::rb::verify()

} // namespace rb
} // namespace impl

//=====================================
template <typename T, typename C, typename K>
const T *
find(const Tree<T, C> &tree, const K &key) noexcept {
  return bst::find(tree, key);
}

//=====================================
template <typename T, typename C, typename K>
T *
find(Tree<T, C> &tree, const K &key) noexcept {
  return bst::find(tree, key);
}

//=====================================
template <typename T, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, C> &tree, K &&value) noexcept {
  using impl::rb::rebalance;
  auto set_root = [&tree](Node<T> *root) {
    if (root->parent == nullptr) {
      tree.root = root;
    }
  };

  auto result = bst::impl::insert(tree, std::forward<K>(value));
  bool inserted{std::get<1>(result)};
  Node<T> *node = std::get<0>(result);
  if (inserted) {
    assertx(node);

    set_root(rebalance(node));
    tree.root->colour = Colour::BLACK;
  }

  T *insval = nullptr;
  if (node) {
    insval = &node->value;
  }

  return std::make_tuple(insval, inserted);
} // rb::insert()

//=====================================
template <typename T, typename C, typename... Arg>
std::tuple<T *, bool>
emplace(Tree<T, C> &self, Arg &&... args) noexcept {
  // TODO make actual impl
  return insert(self, T(std::forward<Arg>(args)...));
} // rb::emplace()

//=====================================
template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &, const K &) noexcept {
  assertx(false);
  // TODO
  return true;
} // rb::remove()

//=====================================
template <typename T, typename C>
bool
verify(Tree<T, C> &tree) noexcept {
  Node<T> *root = tree.root;
  if (root) {
    Node<T> *p = nullptr;
    std::size_t min = 0;
    std::size_t max = 0;
    return impl::rb::verify(p, root, min, max);
  }
  return true;
} // rb::insert()

//=====================================
template <typename T, typename C>
void
dump(Tree<T, C> &tree, const std::string &prefix) noexcept {
  return bst::impl::dump(tree.root, prefix);
}

//=====================================
} // namespace rb

#endif
