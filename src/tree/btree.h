#ifndef SP_UTIL_TREE_BTREE_H
#define SP_UTIL_TREE_BTREE_H

#include <collection/Array.h>

namespace btree {
namespace impl {
namespace btree {
template <typename T, std::size_t rank>
struct Node {
  using value_type = T;

  sp::UinStaticArray<T, rank> elements;
  // Node<T, rank> *left;
  // Node<T, rank> *right;

  Node() noexcept;
};
}
}

/*
 * ==========================================================================
 */
namespace impl {
namespace btree {
template <typename T, std::size_t rank>
Node<T, r>::Node() noexcept
    : elements()
    // , left(nullptr)
    // , right(nullptr)
    {
}
}
}
}
#endif
