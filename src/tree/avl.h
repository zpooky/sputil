#ifndef SP_UTIL_TREE_AVL_TREE_H
#define SP_UTIL_TREE_AVL_TREE_H

#include <cstdint>
#include <tree/tree.h>
#include <tuple>
#include <util/assert.h>
#include <utility>

#include <iostream>
#include <string> //debug

namespace avl {
/*avl*/

//=====================================
template <typename T>
struct Node {
  using value_type = T;

  Node<T> *left;
  Node<T> *right;
  Node<T> *parent;
  T value;
  std::int8_t balance;

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
Node<T>::Node(K &&v, Node<T> *p) noexcept
    : left(nullptr)
    , right(nullptr)
    , parent(p)
    , value(std::forward<K>(v))
    , balance(0) {
}

template <>
inline Node<int>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::to_string(int(value)));
  s.append("|b:");
  s.append(std::to_string(balance));
  s.append("]");
  return s;
}

template <typename T>
Node<T>::operator std::string() const {
  std::string s;
  s.append("[v:");
  s.append(std::string(value));
  s.append("|b:");
  s.append(std::to_string(balance));
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

//=====================================
enum class Direction : bool { LEFT, RIGHT };

template <typename T>
static Direction
direction(const Node<T> *const child) noexcept {
  assertx(child);
  Node<T> *const parent = child->parent;
  assertx(parent);

  if (parent->left == child) {
    return Direction::LEFT;
  }

  assertx(parent->right == child);
  return Direction::RIGHT;
} // avl::impl::avl::direction()

//=====================================
template <typename T>
void
dump_root(Node<T> *tree, std::string prefix = "") noexcept {
  Node<T> *root = tree;
Lstart:
  if (root->parent) {
    root = root->parent;
    goto Lstart;
  }
  return bst::impl::dump(root, prefix);
} // avl::impl::avl::dump_root()

//=====================================
template <typename T>
static std::int8_t
balance(const Node<T> *const node) noexcept {
  return node ? node->balance : 0;
} // avl::impl::avl::balance()

//=====================================
template <typename T>
static Node<T> *
rotate_left(Node<T> *const A) noexcept {
  // printf("\trotate_left(%s)\n", std::string(*A).c_str());
  // dump_root(A, "\t");
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

  assertx(bst::impl::doubly_linked(A));
  assertx(bst::impl::doubly_linked(B));

  // assertx(bst::impl::doubly_linked(A_parent));
  assertx(bst::impl::doubly_linked(B_left));

  // since B can be null when B is null C is as well therefore in that case A
  // will be the root node.
  return B ? B : A;
} // avl::impl::avl::rotate_light()

//=====================================
template <typename T>
static Node<T> *
rotate_right(Node<T> *const C) noexcept {
  // printf("\trotate_right(%s)\n", std::string(*C).c_str());
  // dump_root(C, "\t");
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

  assertx(bst::impl::doubly_linked(B));
  assertx(bst::impl::doubly_linked(C));

  assertx(bst::impl::doubly_linked(B_right));
  // assertx(bst::impl::doubly_linked(C_parent));

  return B;
} // avl::impl::avl::rotate_right()

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
    return parent->left;
  }

  assertx(parent->right == child);
  return parent->right;
} // avl::impl::avl::set()

//=====================================
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

    assertx(bl == b);
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

//=====================================
template <typename T>
void
exchange(Node<T> *node, Node<T> *n) noexcept {
  auto *parent = node->parent;
  if (parent) {
    if (parent->left == node) {
      parent->left = n;
    }
    if (parent->right != node) {
      assertx(parent->right == node);
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

//=====================================
template <typename T>
static bool
verify(Node<T> *const tree) {
  if (!tree) {
    return true;
  }

  std::uint32_t balance = 0;
  return verify(tree->parent, tree, balance);
} // avl::impl::avl::verify()

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
} // avl::impl::avl::verify_root()

namespace insert {
/*avl::impl::avl::insert*/

//=====================================
template <typename T>
static std::size_t
insert_parent_balance(Node<T> *const child) noexcept {
  Node<T> *parent = child->parent;
  Direction d = direction(child);

  if (d == Direction::LEFT) {
    parent->balance--;
  } else {
    parent->balance++;
  }
  return parent->balance;
}

// - The retracing can stop if the balance factor becomes 0 implying that the
//   height of that subtree remains unchanged.
// - If balance factor becomes -1 or +1 continue retraceing
// - If balance factor becomes -2 or +2 we need to repair.
//   After which the subtree has the same height as before
template <typename T, typename F>
static Node<T> *
rebalance(Node<T> *it, F update_parent_balance) noexcept {
  Node<T> *current = nullptr;
Lstart:
  if (it) {
    current = it;

    /* Left Heavy */
    if (balance(current) == -2) {
      if (balance(current->left) == 1) {
        current->left = rotate_left(current->left);
      }

      // update parent with new child
      set(current) = rotate_right(current);

      return current;
    }
    /* Right Heavy */
    else if (balance(current) == 2) {
      if (balance(current->right) == -1) {
        current->right = rotate_right(current->right);
      }

      set(current) = rotate_left(current);

      return current;
    }

    if (current->parent) {
      if (update_parent_balance(current) == 0) {
        return current;
      }
    }

    it = current->parent;
    goto Lstart;
  }

  return current;
} // avl::impl::rebalance()

template <typename T, typename F>
static Node<T> *
rebalance2(Node<T> *it, F setBalance) noexcept {
  Node<T> *current = nullptr;
Lstart:
  if (it) {
    current = it;

    setBalance(current);

    /* Left Heavy */
    if (balance(current) == -2) {
      if (balance(current->left) == 1) {
        current->left = rotate_left(current->left);
      }

      // update parent with new child
      set(current) = rotate_right(current);
    }
    /* Right Heavy */
    else if (balance(current) == 2) {
      if (balance(current->right) == -1) {
        current->right = rotate_right(current->right);
      }

      set(current) = rotate_left(current);
    }

    if (!current->parent) {
      return current;
    }

    it = current->parent;
    goto Lstart;
  }

  return current;
} // avl::impl::rebalance()

} // namespace insert

namespace remove {
/*avl::impl::avl::remove*/
template <typename T>
static Node<T> *
remove_rebalance(Node<T> *current, Node<T> *P = nullptr) noexcept {
  if (current) {
    printf("rebalance current: %s\n", std::string(*current).c_str());
    dump_root(current, "rebalance");
  }
  auto parent = [](Node<T> *n) { return n ? n->parent : nullptr; };
  auto BalanceFactor = [](Node<T> *n) -> std::int8_t & { return n->balance; };
  auto right_child = [](Node<T> *n) { return n->right; };
  auto left_child = [](Node<T> *n) { return n->left; };
  P = P == nullptr ? parent(current) : P;

  // Node<T>* P = parent(current);
  for (; P != nullptr; P = parent(current)) {
    if (current == P->left) { //$current is left child
      printf("$current%s is left child\n",
             current ? std::string(*current).c_str() : "[nullptr]");
      if (BalanceFactor(P) > 0) {
        printf("BalanceFactor(P%s) > 0\n", std::string(*P).c_str());
        auto sibling = right_child(P);
        if (BalanceFactor(sibling) < 0) {
          printf("BalanceFactor(sibling%s) < 0", std::string(*sibling).c_str());
          P->right = rotate_right(
              P->right); // Double rotation: Right(sibling) then Left(P)
          assertx(P->right->parent == P);
        }
        set(P) = rotate_left(P);
        current = P; // sp?
      } else {
        if (BalanceFactor(P) == 0) {
          BalanceFactor(P) += 1; // current’s height decrease is absorbed at P.
          return current;
        }
        current = P;
        BalanceFactor(current) = 0; // Height(current) decreases by 1
      }
      dump_root(current, "rebalance");
    } else if (current == P->right) { // current is right child
      printf("$current%s is right child\n",
             current ? std::string(*current).c_str() : "[nullptr]");
      if (BalanceFactor(P) < 0) { // P is left-heavy
        printf("BalanceFactor(P%s) < 0\n", std::string(*P).c_str());
        // ===> the temporary BalanceFactor(P) == –2
        // ===> rebalancing is required.
        auto sibling = left_child(P);     // Sibling of current (higher by 2)
        if (BalanceFactor(sibling) > 0) { // Left Right Case
          printf("BalanceFactor(sibling%s) > 0\n",
                 std::string(*sibling).c_str());
          P->left = rotate_left(
              P->left); // Double rotation: Left(sibling) then Right(P)
          assertx(P->left->parent == P);
        }
        set(P) = rotate_right(P); // Single rotation Right(P)
        current = P;              // sp?
      } else {
        if (BalanceFactor(P) == 0) {
          printf("BalanceFactor(P%s) == 0\n", std::string(*P).c_str());
          BalanceFactor(P) -= 1; // current’s height decrease is absorbed at P.
          return current;
        }
        current = P;
        BalanceFactor(current) = 0;
      }
      dump_root(current, "rebalance");
    } else {
      assertx(false);
    }
  }
  return current;
} // avl::impl::avl::remove_rebalance()

// test
template <typename T>
static ssize_t
height(const Node<T> *current) {
  if (!current) {
    return 0;
  }
  return 1 + std::max(height(current->right), height(current->left));
}

template <typename T>
static std::int8_t
calc_balance(const Node<T> *current) {
  auto r = height(current->right);
  auto l = height(current->left);
  // if(r==-1)
  //   r = 0;
  // if(l==-1)
  //   l=0;

  return r - l;
}

// template <typename T>
// static std::size_t
// remove_parent_balance(Node<T> *const child) noexcept {
//   Node<T> *parent = child->parent;
//   Direction d = direction(child);
//
//   auto par_bal = calc_balance(parent);
//   if (d == Direction::LEFT) {
//     if (parent->right)
//       parent->balance++;
//   } else {
//     if (parent->left)
//       parent->balance--;
//   }
//   if(parent->balance != par_bal){
//     printf("parent->balance[%d]\npar_bal[%d]\n",parent->balance,par_bal);
//     assertx(parent->balance == par_bal);
//   }
//
//   return parent->balance;
// } // avl::impl::avl::remove::remove_parent_balance()

//=====================================
template <typename T>
static std::size_t
remove_parent_balance(Node<T> *const child) noexcept {
  // Node<T> *parent = child->parent;
  // Direction d = direction(child);

  // if (d == Direction::LEFT) {
  //   parent->balance++;
  // } else {
  //   parent->balance--;
  // }
  child->balance = calc_balance(child);
  return child->balance;
}

template <typename T>
/*new root*/ Node<T> *
remove(Node<T> *const current) noexcept {
  assertx(current);

  auto update_ParentToChild = [](Node<T> *subject, Node<T> *nev) {
    // update parent -> child
    Node<T> *const parent = subject->parent;
    if (parent) {
      if (parent->left == subject) {
        parent->left = nev;
      } else {
        assertx(parent->right == subject);
        parent->right = nev;
      }
    }
  };
  auto unset = [](Node<T> *subject) {
    subject->parent = nullptr;
    subject->left = nullptr;
    subject->right = nullptr;
  };

  printf("remove(%d)", current->value);
  assertx(bst::impl::doubly_linked(current));
  if /*two children*/ (current->left && current->right) {
    printf(":2->");

    /*     X
     *    / \
     * min   max
     *      /
     *     min in the max branch
     */
    /*
     * find the smallest value in the _right_ (greater) branch which will be
     * removed and inserted in the current position.
     * Since we removed a node from the right branch we have to rebalance the
     * tree, but beacuse we only swap out current with the removed node we do
     * not change the balance of the second step.
     */
    Node<T> *const successor = bst::impl::find_min(current->right);
    assertx(bst::impl::doubly_linked(successor));
    Node<T> *const new_root = remove(successor);
    // dump_root(current, "lr");
    // assertx(verify_root(current));
    {
      /*
       * remove($successor) might run rebalancing meaning that we cannot assume
       * that $current
       * have left and right pointers
       */
      update_ParentToChild(current, successor);
      successor->parent = current->parent;
      if (current->left) {
        successor->left = current->left;
        successor->left->parent = successor;
      }

      if (current->right) {
        successor->right = current->right;
        successor->right->parent = successor;
      }
      successor->balance = current->balance;

      assertx(bst::impl::doubly_linked(successor));
      assertx(bst::impl::doubly_linked(successor->left));
      assertx(bst::impl::doubly_linked(successor->right));
    }
    unset(current);
    return new_root == current ? successor : new_root;

    // return insert::rebalance2(successor, [](Node<T> *child) {
    //   return remove_balance(child);
    // });
  } else if /*zero children*/ (!current->left && !current->right) {
    printf(":0\n");
    // zero children
    // auto parent_direction = [](Node<T> *n) {
    //   if (n->parent) {
    //     return direction(n);
    //   }
    //   return Direction::RIGHT;
    // };
    //
    // auto has_sibling = [](Node<T> *n) {
    //   Node<T> *parent = n->parent;
    //   if (parent) {
    //     assertx(parent->left == n || parent->right == n);
    //     std::size_t children = 0;
    //     if (parent->left)
    //       children++;
    //     if (parent->right)
    //       children++;
    //     return children == 2;
    //   }
    //   return false;
    // };

    Node<T> *const parent = current->parent;
    assertx(verify(parent));
    // bool sibling = has_sibling(current);

    // Direction d = parent_direction(current);
    {
      update_ParentToChild(current, (Node<T> *)nullptr);
      assertx(bst::impl::doubly_linked(current->parent));
    }

    unset(current);

    // if (parent) {
    //   #<{(|
    //    * Since we remove a leaf we change the balance of the parent node
    //    |)}>#
    //   if (sibling) {
    //     if (d == Direction::RIGHT) {
    //       parent->balance--;
    //     } else {
    //       parent->balance++;
    //     }
    //   }
    // auto bl = calc_balance(parent);//sp
    // printf("calc_balance(parent%s): %d\n",std::string(*parent).c_str(),bl);
    // parent->balance=bl;

    // return remove_rebalance((Node<T>*)nullptr,parent);
    return insert::rebalance2(
        parent, [](Node<T> *child) { return remove_parent_balance(child); });
    // return parent;
    // }

    // We just removed the last node in the tree
    // return nullptr;
  } else if (current->left) {
    printf(":left\n");
    // one child

    auto *const left = current->left;
    {
      update_ParentToChild(current, left);
      Node<T> *const parent = current->parent;
      left->parent = parent;

      assertx(bst::impl::doubly_linked(parent));
    }

    unset(current);

    // Node<T> *const parent = left->parent;
    // if(parent){
    //   auto bl= calc_balance(parent);//sp
    //   printf("calc_balance(parent%s): %d\n",std::string(*parent).c_str(),bl);
    //   parent->balance =bl;
    // }
    // assertx(left->balance == calc_balance(left));

    // return remove_rebalance(left);
    return insert::rebalance2(
        left, [](Node<T> *child) { return remove_parent_balance(child); });
  }
  assertx(current->right);
  printf(":right\n");
  // one child

  auto *const right = current->right;
  {
    update_ParentToChild(current, right);
    Node<T> *const parent = current->parent;
    right->parent = parent;

    assertx(bst::impl::doubly_linked(parent));
  }

  // Node<T> *const parent = right->parent;
  // if(parent){
  //   auto bl= calc_balance(parent);//sp
  //   printf("calc_balance(parent%s): %d\n",std::string(*parent).c_str(),bl);
  //   right->balance =bl;
  // }
  // assertx(right->balance == calc_balance(right));

  unset(current);
  // return remove_rebalance(right);
  return insert::rebalance2(right, [](Node<T> *child) { //
    return remove_parent_balance(child);
  });
} // avl::impl::avl::remove::remove()

} // namespace remove

} // namespace avl
} // namespace impl

//=====================================
template <typename T, typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, C> &tree, K &&value) noexcept {
  using namespace impl::avl::insert;

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

    set_root(rebalance(node, [](Node<T> *child) { //
      return insert_parent_balance(child);
    }));
    assertx(tree.root->parent == nullptr);
  }

  T *insval = nullptr;
  if (node) {
    insval = &node->value;
  }

  return std::make_tuple(insval, inserted);
} // avl::insert()

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
find(const Tree<T, C> &tree, const K &key) noexcept {
  return bst::find(tree, key);
} // avl::find()

template <typename T, typename C, typename K>
T *
find(Tree<T, C> &tree, const K &key) noexcept {
  return bst::find(tree, key);
} // avl::find()

//=====================================
template <typename T, typename C, typename K>
bool
remove(Tree<T, C> &tree, const K &k) noexcept {
  using avl::impl::avl::remove::remove;

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
    set_root(remove(node));
    return true;
  }

  return false;
} // avl::remove()

//=====================================
template <typename T, typename C>
void
dump(Tree<T, C> &tree, std::string prefix) noexcept {
  return bst::impl::dump(tree.root, prefix);
} // avl::dump()

//=====================================
template <typename T, typename C>
bool
verify(Tree<T, C> &tree) noexcept {
  std::uint32_t balance = 0;
  return impl::avl::verify((Node<T> *)nullptr, tree.root, balance);
} // avl::verify()

//=====================================
} // namespace avl

#endif
