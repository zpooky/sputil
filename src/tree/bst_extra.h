#ifndef SP_UTIL_TREE_BINARY_SEARCH_TREE_EXTRA_H
#define SP_UTIL_TREE_BINARY_SEARCH_TREE_EXTRA_H

#include <queue/Queue.h>
#include <stack/HeapStack.h>
#include <tree/avl.h>
#include <tree/bst.h>

// maybe should be generic bst

namespace binary {
//============================================================
//===Recursive================================================
//============================================================
namespace rec {
//=====================================
namespace impl {
template <typename T, typename C>
bool
equal(const Node<T> *a, const Node<T> *b) noexcept {
  auto eq = [&]() {
    if (a == nullptr) {
      return b == nullptr;
    } else if (b == nullptr) {
      return false;
    }
    C cmp;
    return cmp(a->value, b->value) == false && cmp(b->value, a->value) == false;
  };

  if (!eq()) {
    return false;
  }

  if (a) {

    if (!equal<T, C>(a->left, b->left)) {
      return false;
    }

    if (!equal<T, C>(a->right, b->right)) {
      return false;
    }
  }

  return true;
} // binary::rec::impl::binary::equal()

} // namespace binary::rec::impl

template <typename T, typename C>
bool
equal(const Tree<T, C> &a, const Tree<T, C> &b) noexcept {
  return impl::equal<T, C>(a.root, b.root);
}

//=====================================
namespace impl {
template <typename T>
std::size_t
height(const Node<T> *node) noexcept {
  if (node) {
    return std::max(height(node->left), height(node->right)) + 1;
  } else {
    return 0;
  }
} // binary::impl::binary::height()
}

template <typename T, typename C>
std::size_t
height(const Tree<T, C> &tree) noexcept {
  return impl::height(tree.root);
}

//=====================================
namespace impl {
template <typename T>
Node<T> *
delete_self(Node<T> *tree) noexcept {
  if (tree) {
    tree->left = delete_self(tree->left);
    tree->right = delete_self(tree->right);
    // we need to null left&right since otherwise the dtor of node will try to
    // delete the already reclaimed memory
    delete tree;
  }
  return nullptr;
}
}

template <typename T, typename C>
void
delete_self(Tree<T, C> &tree) noexcept {
  tree.root = impl::delete_self(tree.root);
}

//=====================================
namespace impl {
template <typename T, typename F>
void
inorder(const T *tree, F f) noexcept {
  if (tree) {
    inorder(tree->left, f);
    f(tree->value);
    inorder(tree->right, f);
  }
}
}

template <typename T, typename C, typename F>
void
inorder(const Tree<T, C> &tree, F f) noexcept {
  return impl::inorder(tree.root, f);
}

template <typename T, typename C, typename F>
void
inorder(const avl::Tree<T, C> &tree, F f) noexcept {
  return impl::inorder(tree.root, f);
}

//=====================================
namespace impl {
template <typename T, typename F>
void
preorder(const Node<T> *tree, F f) noexcept {
  if (tree) {
    f(tree->value);
    preorder(tree->left, f);
    preorder(tree->right, f);
  }
}
}

template <typename T, typename C, typename F>
void
preorder(const Tree<T, C> &tree, F f) noexcept {
  return impl::preorder(tree.root, f);
}

//=====================================
namespace impl {
template <typename T, typename F>
void
postorder(const Node<T> *tree, F f) noexcept {
  if (tree) {
    postorder(tree->left, f);
    postorder(tree->right, f);
    f(tree->value);
  }
}
}

template <typename T, typename C, typename F>
void
postorder(const Tree<T, C> &tree, F f) noexcept {
  return impl::postorder(tree.root, f);
}

//=====================================
namespace impl {
template <typename T, typename F>
bool
levelorder(const Node<T> *tree, F f, std::size_t cnt) noexcept {
  if (tree) {
    if (cnt == 0) {
      f(tree->value);
      return true;
    } else {
      bool l = levelorder(tree->left, f, cnt - 1);
      bool r = levelorder(tree->right, f, cnt - 1);
      return l || r;
    }
  }
  return false;
}
}

template <typename T, typename C, typename F>
void
levelorder(const Tree<T, C> &tree, F f) noexcept {
  // start at the top level and work down
  std::size_t level = 0;
  while (impl::levelorder(tree.root, f, level++))
    ;
}

//=====================================
namespace impl {
template <typename T, typename F>
void
reverse_levelorder(const Node<T> *tree, F f,
                   sp::HeapStack<const Node<T> *> &stack) noexcept {
  if (tree) {
    push(stack, tree);
    reverse_levelorder(tree->left, f, stack);
    reverse_levelorder(tree->right, f, stack);
  }
}
}

template <typename T, typename C, typename F>
void
reverse_levelorder(const Tree<T, C> &tree, F f) noexcept {
  sp::HeapStack<const Node<T> *> stack;
  // start at the bottom level and work up
  impl::reverse_levelorder(tree.root, f, stack);
  for_each(stack, [f](auto *c) {
    /**/
    return f(c->value);
  });
}

//=====================================
template <typename T, typename C, typename F>
void
spiralorder(const Tree<T, C> &, F) noexcept {
  // http://www.techiedelight.com/spiral-order-traversal-binary-tree/
  // TODO
}

//=====================================
template <typename T, typename C, typename F>
void
inverseorder(const Tree<T, C> &, F) noexcept;

//=====================================
template <typename T, typename C>
bool
is_perfect_binary_tree(const Tree<T, C> &) noexcept {
  return false;
}

//=====================================
template <typename T, typename C>
bool
is_complete_binary_tree(const Tree<T, C> &) noexcept {
  // http://www.techiedelight.com/check-given-binary-tree-complete-binary-tree-not/
  // every node is filled except possible last which is empty/full/left
  return false;
}

//=====================================
template <typename T, typename C>
bool
is_sum_tree(const Tree<T, C> &) noexcept {
  // check if the value of a node is the sum of all values for each child nodes
  // http://www.techiedelight.com/check-given-binary-tree-sum-tree-not/
  return false;
}

//=====================================
template <typename T, typename C>
bool
is_symmetric(const Tree<T, C> &) noexcept {
  // http://www.techiedelight.com/check-given-binary-tree-symmetric-structure-not/
  // check the shape of the left subtree is the inverse on the right subtree
  return false;
}

//=====================================
template <typename T, typename C>
bool
is_height_balanced(const Tree<T, C> &) noexcept {
  // http://www.techiedelight.com/check-given-binary-tree-is-height-balanced-not/
  // is the difference in height either 0/1 between the left and right child
  return false;
}

//=====================================
namespace impl {
template <typename T, typename C>
bool
is_binary_tree_bst(const Node<T> *tree) noexcept {
  // right > parent
  // left < parent
  if (tree) {
    C cmp;
    if (tree->right) {
      if (!cmp(tree->right->value, tree->value)) {
        return false;
      }
    }

    if (tree->left) {
      if (!cmp(tree->value, tree->left->value)) {
        return false;
      }
    }

    return is_binary_tree_bst<T, C>(tree->left) &&
           is_binary_tree_bst<T, C>(tree->right);
  }

  return true;
}
}

template <typename T, typename C>
bool
is_binary_tree_bst(const Tree<T, C> &tree) noexcept {
  // right > parent
  // left < parent
  return impl::is_binary_tree_bst<T, C>(tree.root);
}

//=====================================
namespace impl {
template <typename T>
std::size_t
max_width(const Node<T> *tree, std::size_t n = 0) noexcept {
  // TODO this wont work it is better to find the number of levels and calculate
  // the max widht
  if (tree) {
    // n = n == 0 ? 1 : n;
    if (tree->left || tree->right)
      return std::max(max_width(tree->left, n + 2),
                      max_width(tree->right, n + 2));
  }
  return n;
}
} // namespace rec::impl

template <typename T, typename C>
std::size_t
max_width(const Tree<T, C> &tree) noexcept {
  // http://www.techiedelight.com/find-maximum-width-given-binary-tree/
  return impl::max_width(tree.root);
}

} // namespace binary::rec

//=====================================
template <typename T, typename C>
void
print_cousin_nodes(const Tree<T, C> &) noexcept {
  // http: // www.techiedelight.com/print-cousins-of-given-node-binary-tree/
}

//=====================================
template <typename T, typename C>
void
mirror(const Tree<T, C> &) noexcept {
  // http://www.techiedelight.com/convert-binary-tree-to-its-mirror/
}

//=====================================
template <typename T, typename C>
void
lowest_common_ancestor(const Tree<T, C> &) noexcept {
  // http://www.techiedelight.com/find-lowest-common-ancestor-lca-two-nodes-binary-tree/
}

//=====================================
template <typename T, typename C, typename F>
void
level_first_left(const Tree<T, C> &, F) noexcept {
  // http://www.techiedelight.com/print-left-view-of-binary-tree/
}

//=====================================
// Unbalanced Tree -> Balanced Tree
template <typename T, typename C>
bool
balance(Tree<T, C> &) noexcept;

//=====================================
template <typename T, typename C>
bool
reverse(Tree<T, C> &) noexcept;

//=====================================
namespace rec {} // namespace binary::rec

//=====================================

//============================================================
//===Itterative===============================================
//============================================================
namespace it {
template <typename T, typename C, typename F>
void
levelorder(const Tree<T, C> &tree, F f) noexcept {
  if (!tree.root) {
    return;
  }
  sp::LinkedListQueue<const Node<T> *> queue;
  assert(enqueue(queue, tree.root));
  // FIFO queue handled level-wise since all nodes of the same level will be
  // handled before the next because of the FIFO semantics.
  // root[1]  -> add_back(left[2],right[2])
  //
  // left[2]  -> add_back(left[3],right[3])
  // right[2] -> add_back(left[3],right[3])
  //
  // left[3]  -> add_back(left[4],right[4])
  // right[3] -> add_back(left[4],right[4])
  // left[3]  -> add_back(left[4],right[4])
  // right[3] -> add_back(left[4],right[4])
  // ...
  while (!is_empty(queue)) {
    const Node<T> *out = nullptr;
    assert(dequeue(queue, out));
    assert(out);
    const T &value = out->value;
    f(value);

    if (out->left)
      assert(enqueue(queue, out->left));
    if (out->right)
      assert(enqueue(queue, out->right));
  }
}

template <typename T, typename C, typename F>
void
reverse_levelorder(const Tree<T, C> &tree, F f) noexcept {
  if (!tree.root) {
    return;
  }
  sp::HeapStack<Node<T> *> stack;
  sp::LinkedListQueue<Node<T> *> queue;
  assert(enqueue(queue, tree.root));
  /*
   * Works the same as levelorder but instead of applying f over the current
   * node we push it to a LIFO stack meanining the first node will be last in
   * the stack and the the leaf nodes with be at the top of the stack
   */

  while (!is_empty(queue)) {
    Node<T> *out = nullptr;
    assert(dequeue(queue, out));
    assert(out);

    assert(push(stack, out));

    if (out->left)
      assert(enqueue(queue, out->left));
    if (out->right)
      assert(enqueue(queue, out->right));
  }
  for_each(stack, [f](auto *c) {
    /**/
    return f(c->value);
  });
}
} // namepace binary::it

} // namespace binary

#endif
