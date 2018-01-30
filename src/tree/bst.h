#ifndef SP_MALLOC_BINARY_SEARCH_TREE_H
#define SP_MALLOC_BINARY_SEARCH_TREE_H

#include "tree.h"
#include <tuple>

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

/*
 * Unlinks from and replaces it with to
 */
// template <typename T>
// void
// replace(Node<T> *from, Node<T> *to) noexcept {
//   auto parent_child_link = [](Node<T> *subject, Node<T> *nev) {
//     // update parent -> child
//     Node<T> *const parent = subject->parent;
//     if (parent) {
//       if (parent->left == subject) {
//         parent->left = nev;
//       } else {
//         assert(parent->right == subject);
//         parent->right = nev;
//       }
//     }
//   };
//
//   parent_child_link(from, to);
//
//   if (to) {
//     parent_child_link(#<{(|from|)}># to, #<{(|to|)}># nullptr);
//     to->parent = from->parent;
//
//     to->left = from->left;
//     if (to->left)
//       to->left->parent = to;
//
//     to->right = from->right;
//     if (to->right)
//       to->right->parent = to;
//   }
// } // impl::binary::replace()

template <typename T>
Node<T> *
remove(Node<T> *current) noexcept {
  assert(current);
  using namespace bst::impl::bst;

  auto update_ParentToChild = [](Node<T> *subject, Node<T> *replacement) {
    // update parent -> child
    Node<T> *const parent = subject->parent;
    if (parent) {
      if (parent->left == subject) {
        parent->left = replacement;
      } else {
        assert(parent->right == subject);
        parent->right = replacement;
      }
    }
  };

  assert(bst::impl::tree::doubly_linked(current));

  if /*two children*/(current->left && current->right) {
    /*
     * Replace $current with the minimum child found in $current right branch($successor).
     * The $successor is the natural replacement for $current since all BST properties will still hold after the remove:
     * - $successor will be greater than any node in the left branch since $successor was taken from the right branch.
     * - $successor will be less than the node on the right since $successor was the smallest node in the right branch.
     */
    Node<T> *const successor = find_min(current->right);
    {
      /*
       * Unlinks $successor.
       */
      remove(successor);

      /*
       * Update [parent -> child] Link. now [parent -> successor]
       */
      update_ParentToChild(current, successor);
      {
        /*
         * Update $successor relation pointers the be the same as $current had before.
         */
        successor->parent = current->parent;
        successor->left = current->left;
        successor->left->parent = successor;

        if (current->right) {
          /*
           * If the immediate right was not picked as the successor(not null).
           */
          successor->right = current->right;
          successor->right->parent = successor;
        }
      }

      assert(bst::impl::tree::doubly_linked(successor));
    }

    current->parent = nullptr;
    current->left = nullptr;
    current->right = nullptr;

    /*
     * Returns the new root of the sub-Tree
     */
    return successor;
  } else if /*zero children*/(!current->left && !current->right) {
    /*
     * Unset $current from [parent -> child] and replace it with null
     */
    Node<T> *parent = current->parent;
    Node<T> *unset = nullptr;
    update_ParentToChild(current, unset);

    assert(bst::impl::tree::doubly_linked(parent));

    current->parent = nullptr;

    return parent;
  } else if /*left child*/(current->left) {
    /*
     * Unset $current from [parent -> child] and replace it with $current only child
     */
    Node<T> *const parent = current->parent;
    auto *const left = current->left;
    update_ParentToChild(current, left);
    left->parent = parent;

    assert(bst::impl::tree::doubly_linked(parent));

    current->parent = nullptr;
    current->left = nullptr;

    return left;
  }
  assert(current->right);
  /* right child */

  /*
   * Unset $current from [parent -> child] and replace it with $current only child
   */
  Node<T> *const parent = current->parent;
  auto *const right = current->right;
  update_ParentToChild(current, right);
  right->parent = parent;

  assert(bst::impl::tree::doubly_linked(parent));

  current->parent = nullptr;
  current->right = nullptr;

  return right;
} // impl::binary::remove()

} // namespace binary
} // namespace impl
//===================================================

template <typename T,typename C, typename K>
std::tuple<T *, bool>
insert(Tree<T, C> &tree, K &&ins) noexcept {
  if (!tree.root) {
    // insert into empty tree
    tree.root = new (std::nothrow) Node<T>(std::forward<K>(ins));
    if (tree.root) {
      return std::make_tuple(&tree.root->value, true);
    }

    return std::make_tuple(nullptr, false);
  }

  Node<T> *it = tree.root;

Lstart:
  if (ins < it->value) {
    if (it->left) {
      it = it->left;

      goto Lstart;
    }

    it->left = new (std::nothrow) Node<T>(std::forward<K>(ins), it);
    if (it->left) {
      return std::make_tuple(&it->left->value, true);
    }
  } else if (ins > it->value) {
    if (it->right) {
      it = it->right;

      goto Lstart;
    }

    it->right = new (std::nothrow) Node<T>(std::forward<K>(ins), it);
    if (it->right) {
      return std::make_tuple(&it->right->value, true);
    }
  } else {

    return std::make_tuple(&it->value, false);
  }

  return std::make_tuple(nullptr, false);
} // binary::insert()

template <typename T,typename C, typename K>
bool
remove(Tree<T,C> &tree, const K &k) noexcept {
  Node<T> *const node = bst::impl::bst::find_node<Node<T>,C,K>(tree.root, k);
  if (node) {

    Node<T> *const nroot = impl::binary::remove(node);
    if (nroot) {
      if (nroot->parent == nullptr) {
        tree.root = nroot;
      }
    } else {
      tree.root = nullptr;
    }

    delete (node);
    return true;
  }

  return false;
} // binary::remove()

template <typename T,typename C>
void
dump(Tree<T,C> &tree, std::string prefix) noexcept {
  return bst::impl::tree::dump(tree.root, prefix);
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
