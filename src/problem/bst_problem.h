#ifndef SP_UTIL_TREE_BST_PROBLEM_H
#define SP_UTIL_TREE_BST_PROBLEM_H

#include <cstddef>

namespace bstp {

struct Tree {
  Tree *left = nullptr;
  Tree *right = nullptr;
  int value;
};

//=====================================
template <std::size_t N>
bool
tree_to_array(Tree *src, Tree *(&dest)[N], std::size_t &index) noexcept {
  if (src) {
    tree_to_array(src->left, dest, index);
    if (index == N) {
      return false;
    }
    dest[index++] = src;
    tree_to_array(src->right, dest, index);
  }
  return true;
}

//=====================================
inline Tree *
partition_insert(Tree **arr, std::size_t length) noexcept {
  if (length == 0) {
    return nullptr;
  }

  auto mid(length / 2);
  Tree *m = arr[mid];

  m->left = partition_insert(arr, mid);
  m->right = partition_insert(arr + mid + 1, length - (mid + 1));
  return m;
}

//=====================================
inline Tree *
balance(Tree *src) noexcept {
  Tree *arr[256] = {nullptr};
  std::size_t index = 0;
  if (!tree_to_array(src, arr, index)) {
    return nullptr;
  }

  return partition_insert(arr, index);
}

//=====================================
inline std::uint64_t
fib(std::uint64_t n) noexcept {
  if (n <= 0)
    return n;
  return fib(n - 1) + fib(n - 2);
}

//=====================================

}

#endif
