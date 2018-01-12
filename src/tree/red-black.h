#ifndef SP_UTIL_TREE_RB_TREE_H
#define SP_UTIL_TREE_RB_TREE_H

namespace rb {

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

template <typename T, typename K>
std::tuple<T *, bool>
insert(Tree<T> &, K &&) noexcept;

template <typename T, typename K>
bool
remove(Tree<T> &, const K &) noexcept;

}//namespace rb

#endif
