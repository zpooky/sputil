#ifndef SP_UTIL_TREE_RB_TREE_H
#define SP_UTIL_TREE_RB_TREE_H

#include "tree.h"
#include <tuple>
#include <utility>//forward

namespace rb {

  enum class Colour : bool {RED,BLACK};

template <typename T>
struct Node {
  using value_type = T;

  // TODO lesser
  Node<T> *left;
  // TODO greater
  Node<T> *right;
  Node<T> *parent;
  T value;
  Colour colour;

  template <typename K>
  explicit Node(K &&v, Node<T> *p = nullptr)
      : left(nullptr)
      , right(nullptr)
      , parent(p)
      , value(std::forward<K>(v))
      , colour(Colour::RED) {
  }

  explicit operator std::string() const {
    std::string s;
    s.append("[v:");
    s.append(std::to_string(int(value)));
    s.append("|b:");
    s.append(colour == Colour::RED ? "RED" : "");
    s.append(colour == Colour::BLACK ? "BLACK":"");
    s.append(colour != Colour::BLACK && colour != Colour::RED? "ERROR":"");
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
using Tree = sp::Tree<rb::Node<T>>;

template <typename T,typename K>
const T* find(const Tree<T>&,const K&) noexcept;//TODO

template <typename T, typename K>
std::tuple<T *, bool>
insert(Tree<T> &, K &&) noexcept;

template <typename T, typename K>
bool
remove(Tree<T> &, const K &) noexcept;//TODO

template <typename T>
void
dump(Tree<T> &tree, std::string prefix = "") noexcept;//TODO

template <typename T>
bool
verify(Tree<T> &tree) noexcept;//TODO

//==================
namespace impl {
namespace rb {

template<typename T>
static Node<T>*parent(Node<T>*n){
  assert(n);
  return n->parent;
}

template<typename T>
static Node<T>*grandparent(Node<T>*n){
  Node<T>* p = parent(n);
  if(!sp::impl::tree::doubly_linked(p))
    assert(sp::impl::tree::doubly_linked(p));
  return p ? parent(p) : nullptr;
}

template<typename T>
static Node<T>*uncle(Node<T>*n){
  Node<T>*p = parent(n);
  Node<T> *g = grandparent(n);
  if(g->left != p){
    assert(g->right == p);
    return g->left;
  }

  assert(g->left == p);
  return g->right;
}

template<typename T>
static Node<T>* rotate_right(Node<T>*C) noexcept {
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

  assert(sp::impl::tree::doubly_linked(B));
  assert(sp::impl::tree::doubly_linked(C));

  assert(sp::impl::tree::doubly_linked(B_right));

  if(C_parent){
    assert(C_parent->left == C || C_parent->right == C);

    if(C_parent->left == C ){
      C_parent->left = B;
    }else {
      C_parent->right = B;
    }
  }
  return nullptr;
}

template<typename T>
static Node<T>* rotate_left(Node<T>*const A) noexcept {
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

  assert(sp::impl::tree::doubly_linked(A));
  assert(sp::impl::tree::doubly_linked(B));

  assert(sp::impl::tree::doubly_linked(B_left));

  if(A_parent){
    assert(A_parent->left == A || A_parent->right == A);
    if(A_parent->left == A ){
      A_parent->left = B ? B : A;
    }else {
      A_parent->right = B ? B : A;
    }
  }
  return nullptr;
}

template<typename T>
static Node<T>*rebalance(Node<T>*n) {
  assert(n->colour == Colour::RED);

  Node<T>*p = parent(n);
  if(!p){
    n->colour = Colour::BLACK;
    return n;
  }

  if(p->colour == Colour::BLACK){
    return nullptr;
  } else if(p->colour == Colour::RED) {
    Node<T> *u = uncle(n);

    if(u && u->colour == Colour::RED){
      Node<T>*g = grandparent(n);
      assert(g);

      p->colour =Colour::BLACK;
      u->colour =Colour::BLACK;
      g->colour = Colour::RED;

      return rebalance(g);
    }else {
      {
        Node<T> * g = grandparent(n);
        Node<T> *p = parent(n);
        if(g && n == g->left->right){
          assert(p);

          rotate_left(p);
          n = n->left;
        }else if (g && n == g->right->left){
          assert(p);

          rotate_right(p);
          n = n->right;
        }
      }

      {
        Node<T>*p=parent(n);
        assert(p);
        if(n == p->left){
          rotate_right(grandparent(n));
        }else {
          rotate_left(grandparent(n));
        }
      }

      {
        Node<T> *g = grandparent(n);
        Node<T> *p = parent(n);
        // assert(g);
        assert(p);

        p->colour = Colour::BLACK;
        if(g)
          g->colour = Colour::RED;
      }
      return nullptr;
    }
  }
  assert(false);
}//rb::impl::rb::rebalance()

template<typename T>
static bool verify(Node<T>*parent,Node<T>*current){
  if(parent){
    if(parent->colour == Colour::RED){
      if(current->colour!=Colour::BLACK){
        return false;
      }
    }

    if(current->parent != parent){
      return false;
    }
  }else {
    if(current->colour != Colour::BLACK){
      return false;
    }
  }

  if(current->left){
    if(!(current->left->value < current->value )){
      return false;
    }
    if(!verify(current,current->left)){
      return false;
    }
  }

  if(current->right){
    if(!(current->right->value > current->value)){
      return false;
    }
    if(!verify(current,current->right)){
      return false;
    }
  }
  return true;
}//rb::impl::rb::verify()

}//namespace rb
}//namespace impl

//==================
template <typename T, typename K>
std::tuple<T *, bool>
insert(Tree<T> &tree, K &&ins) noexcept {
  auto set_root = [&tree](Node<T> *root) {
    if (root) {
      tree.root = root;
    }
  };

  if (!tree.root) {
    /*Insert into empty tree*/
    tree.root = new (std::nothrow) Node<T>(std::forward<K>(ins));
    if (tree.root) {
      tree.root->colour = Colour::BLACK;
      return std::make_tuple(&tree.root->value, true);
    }

    return std::make_tuple(nullptr, false);
  }

  // TODO share with bst
  Node<T> *it = tree.root;

Lstart:
  /*Ordinary Binary Insert*/
  if (ins < it->value) {
    if (it->left) {
      it = it->left;

      goto Lstart;
    }

    auto res = it->left = new (std::nothrow) Node<T>(std::forward<K>(ins), it);
    if (it->left) {
      set_root(impl::rb::rebalance(it->left));

      return std::make_tuple(&res->value, true);
    }
  } else if (ins > it->value) {
    if (it->right) {
      it = it->right;

      goto Lstart;
    }

    auto res = it->right = new (std::nothrow) Node<T>(std::forward<K>(ins), it);
    if (it->right) {
      set_root(impl::rb::rebalance(it->right));

      return std::make_tuple(&res->value, true);
    }
  } else {

    return std::make_tuple(&it->value, false);
  }

  return std::make_tuple(nullptr, false);
}//rb::insert()

template <typename T>
bool
verify(Tree<T> &tree) noexcept {
  Node<T> * root = tree.root;
  if(root){
    Node<T>*p = nullptr;
    return impl::rb::verify(p,root);
  }
  return true;
}//rb::insert()

}//namespace rb

#endif
