#ifndef P_UTIL_TREE_AVL_TREE_H
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

  Node<T> *left;
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

template <typename T,typename Comparator = sp::greater>
using Tree = bst::Tree<avl::Node<T>, Comparator>;

template <typename T,typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T,C> &, K &&) noexcept;

template <typename T,typename C,typename K>
const T* find(const Tree<T,C>&,const K&) noexcept;

template <typename T,typename C,typename K>
T* find(Tree<T,C>&,const K&) noexcept;

template <typename T,typename C, typename K>
bool
remove(Tree<T,C> &, const K &) noexcept;

template <typename T,typename C>
void
dump(Tree<T,C> &tree, std::string prefix = "") noexcept;

template <typename T,typename C>
bool
verify(Tree<T,C> &tree) noexcept;

/*
 * ==========================================================================
 */

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
  return bst::impl::tree::dump(root, prefix);
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

  assert(bst::impl::tree::doubly_linked(A));
  assert(bst::impl::tree::doubly_linked(B));

  // assert(bst::impl::tree::doubly_linked(A_parent));
  assert(bst::impl::tree::doubly_linked(B_left));

  //since B can be null when B is null C is as well therefore in that case A
  //will be the root node.
  return B ? B : A;
}// avl::impl::avl::rotate_light()

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

  assert(bst::impl::tree::doubly_linked(B));
  assert(bst::impl::tree::doubly_linked(C));

  assert(bst::impl::tree::doubly_linked(B_right));
  // assert(bst::impl::tree::doubly_linked(C_parent));

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

namespace insert {
/*av::impl::avl::insert*/

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
retrace(Node<T> *it, F update_parent_balance) noexcept {
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

      /*
       * If there is _no_ parent then current is root.
       * If there is a parent then we have not altered the root node.
       */
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
} // avl::impl::retrace()

}//namespace insert

namespace remove {
/*avl::impl::avl::remove*/
template <typename T, typename F>
static Node<T> *
retrace(Node<T> *it, F update_parent_balance) noexcept {
  // return impl::avl::ins::retrace(it, update_parent_balance);
  //   Node<T> *current = nullptr;
  // Lstart:
  //   if (it) {
  //     current = it;
  //     {
  //       printf("---%s\n", std::string(*it).c_str());
  //       dump_root(it, "retrace|");
  //     }
  //     // https://www.geeksforgeeks.org/avl-tree-set-2-deletion/
  //
  //     it = current->parent;
  //     goto Lstart;
  //   }
  //   // TODO
  //   return current;
  return nullptr;
} // avl::impl::retrace()

  //test
template <typename T>
static ssize_t height(const Node<T>*current){
  if(!current){
    return -1;
  }
  return 1+ std::max(height(current->right),height(current->left));
}

template <typename T>
static std::uint8_t calc_balance(const Node<T>*current){
  auto r =height(current->right) ;
  auto l  =height(current->left);
  if(r==-1)
    r = 0;
  if(l==-1)
    l=0;

  return r-l;
}

template <typename T>
static std::size_t
remove_parent_balance(Node<T> *const child) noexcept {
  Node<T> *parent = child->parent;
  Direction d = direction(child);

  auto par_bal = calc_balance(parent);
  if (d == Direction::LEFT) {
    if (parent->right)
      parent->balance++;
  } else {
    if (parent->left)
      parent->balance--;
  }
  if(parent->balance != par_bal){
    printf("parent->balance[%d]\npar_bal[%d]\n",parent->balance,par_bal);
    assert(parent->balance == par_bal);
  }

  return parent->balance;
} // avl::impl::avl::remove::remove_parent_balance()

template <typename T>
/*new root*/ Node<T> *
remove(Node<T> *const current) noexcept {
  assert(current);

  auto update_ParentToChild = [](Node<T> *subject, Node<T> *nev) {
    // update parent -> child
    Node<T> *const parent = subject->parent;
    if (parent) {
      if (parent->left == subject) {
        parent->left = nev;
      } else {
        assert(parent->right == subject);
        parent->right = nev;
      }
    }
  };
  auto unset = [](Node<T>*subject){
    subject->parent = nullptr;
    subject->left = nullptr;
    subject->right = nullptr;
  };

  // TODO update balance factor when replaceing current(the new should have the
  // current balance since we only replace without changing balance)

  printf("remove(%d)", current->value);
  assert(bst::impl::tree::doubly_linked(current));
  if /*two children*/(current->left && current->right) {
    // two children
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
    assert(bst::impl::tree::doubly_linked(successor));
    Node<T> *const new_root = remove(successor);
    dump_root(current, "lr");
    // assert(verify_root(current));
    {
      /*
       * remove might run a retrace meaning that we can not assume that current
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

      assert(bst::impl::tree::doubly_linked(successor));
      assert(bst::impl::tree::doubly_linked(successor->left));
      assert(bst::impl::tree::doubly_linked(successor->right));
    }
    unset(current);
    return new_root;
  } else if (!current->left && !current->right) {
    printf(":0\n");
    // zero children
    auto parent_direction = [](Node<T> *n) {
      if (n->parent) {
        return direction(n);
      }
      return Direction::RIGHT;
    };

    auto has_sibling = [](Node<T> *n) {
      Node<T> *parent = n->parent;
      if (parent) {
        assert(parent->left == n || parent->right == n);
        std::size_t children = 0;
        if (parent->left)
          children++;
        if (parent->right)
          children++;
        return children == 2;
      }
      return false;
    };

    Node<T> *const parent = current->parent;
    assert(verify(parent));
    bool sibling = has_sibling(current);

    Direction d = parent_direction(current);
    {
      update_ParentToChild(current, (Node<T> *)nullptr);
      assert(bst::impl::tree::doubly_linked(current->parent));
    }

    if (parent) {
      /*
       * Since we remove a leaf we change the balance of the parent node
       */
      if (sibling) {
        if (d == Direction::RIGHT) {
          parent->balance--;
        } else {
          parent->balance++;
        }
      }

      return retrace(parent, [](Node<T> *child) { //
        return remove_parent_balance(child);
      });
    }

    unset(current);

    // We just removed the last node in the tree
    return nullptr;
  } else if (current->left) {
    printf(":left\n");
    // one child

    auto *const left = current->left;
    {
      update_ParentToChild(current, left);
      Node<T> *const parent = current->parent;
      left->parent = parent;

      assert(bst::impl::tree::doubly_linked(parent));
    }

    unset(current);
    return retrace(left, [](Node<T> *child) { //
      return remove_parent_balance(child);
    });
  }
  assert(current->right);
  printf(":right\n");
  // one child

  auto *const right = current->right;
  {
    update_ParentToChild(current, right);
    Node<T> *const parent = current->parent;
    right->parent = parent;

    assert(bst::impl::tree::doubly_linked(parent));
  }

  unset(current);
  return retrace(right, [](Node<T> *child) { //
    return remove_parent_balance(child);
  });
} // avl::impl::avl::remove::remove()

}//namespace remove

} // namespace impl
} // namespace avl

template <typename T,typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T,C> &tree, K &&value) noexcept {
  using namespace impl::avl::insert;

  auto set_root = [&tree](Node<T> *root) {
    if(root->parent == nullptr){
      tree.root = root;
    }
  };

  auto result = bst::impl::insert(tree, std::forward<K>(value));
  bool inserted{std::get<1>(result)};
  Node<T>* node = std::get<0>(result);
  if(inserted){
    assert(node);

    set_root(retrace(node, [](Node<T> *child) {
      return insert_parent_balance(child);
    }));
  }

  T* insval = nullptr;
  if(node){
    insval = &node->value;
  }

  return std::make_tuple(insval, inserted);
}//avl::insert()

template <typename T,typename C,typename K>
const T* find(const Tree<T,C>&tree,const K&key) noexcept {
  return bst::find(tree,key);
}//av::find()

template <typename T,typename C,typename K>
T* find(Tree<T,C>&tree,const K&key) noexcept {
  return bst::find(tree,key);
}//av::find()

template <typename T,typename C, typename K>
bool
remove(Tree<T,C> &tree, const K &k) noexcept {
  Node<T> *const node = bst::impl::find_node<T,C,K>(tree.root, k);

  if (node) {
    Node<T> *const new_root = avl::impl::avl::remove::remove(node);

    if (new_root) {
      tree.root = new_root;
    } else {
      if (tree.root == node) {
        tree.root = nullptr;
      }
    }

    return true;
  }

  return false;
} // avl::remove()

template <typename T,typename C>
void
dump(Tree<T,C> &tree, std::string prefix) noexcept {
  return bst::impl::tree::dump(tree.root, prefix);
}//avl::dump()

template <typename T,typename C>
bool
verify(Tree<T,C> &tree) noexcept {
  std::uint32_t balance = 0;
  return impl::avl::verify((Node<T> *)nullptr, tree.root, balance);
}//avl::verify()

} // namespace avl

#endif
