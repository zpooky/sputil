#ifndef SP_UTIL_TREE_STATIC2_TREE_H
#define SP_UTIL_TREE_STATIC2_TREE_H

#include <tree/tree.h>
#include <tuple>
#include <util/comparator.h>

namespace bst {
template <typename T, typename Comparator = sp::less,
          typename Present = sp::present>
struct StaticTree {
  T *buffer;
  const std::size_t capacity;

  StaticTree(T *, std::size_t) noexcept;

  template <std::size_t SIZE>
  explicit StaticTree(T (&)[SIZE]) noexcept;
};

template <typename T, typename C, typename P, typename V>
std::tuple<T *, bool>
insert(StaticTree<T, C, P> &, V &&) noexcept;

template <typename T, typename C, typename P, typename K>
const T *
find(const StaticTree<T, C, P> &, const K &) noexcept;

template <typename T, typename C, typename P, typename K>
const T *
find(StaticTree<T, C, P> &, const K &) noexcept;

template <typename T, typename C, typename P, typename K>
bool
remove(StaticTree<T, C, P> &, const K &) noexcept;

template <typename T, typename C, typename P>
bool
verify(StaticTree<T, C, P> &) noexcept;

template <typename T, typename C, typename P>
void
dump(const StaticTree<T, C, P> &, const std::string & = "") noexcept;
//==================
namespace impl {
namespace StaticTree {

static std::size_t
parent(std::size_t idx) noexcept {
  return (idx - 1) / 2;
}

static std::size_t
left_child(std::size_t idx) noexcept {
  return (2 * idx) + 1;
}

static std::size_t
right_child(std::size_t idx) noexcept {
  return (2 * idx) + 2;
}

template <typename T, typename V>
static T *
make(T *buffer, std::size_t idx, V &&val) noexcept {
  T *const dest = buffer + idx;
  dest->~T();
  new (dest) T(std::forward<V>(val));
  return dest;
}

} // namespace StaticTree
} // namespace impl

template <typename T, typename C, typename P>
StaticTree<T, C, P>::StaticTree(T *b, std::size_t l) noexcept
    : buffer(b)
    , capacity(l) {
}

template <typename T, typename C, typename P>
template <std::size_t SIZE>
StaticTree<T, C, P>::StaticTree(T (&buffer)[SIZE]) noexcept
    : StaticTree(buffer, SIZE) {
}

template <typename T, typename C, typename P, typename V>
std::tuple<T *, bool>
insert(StaticTree<T, C, P> &tree, V &&v) noexcept {
  using namespace impl::StaticTree;

  std::size_t idx = 0;
Lit:
  if (idx < tree.capacity) {
    constexpr P present;
    if (present(tree.buffer[idx])) {
      constexpr C cmp;
      if (cmp(v, tree.buffer[idx])) {
        idx = right_child(idx);
      } else if (cmp(tree.buffer[idx], v)) {
        idx = left_child(idx);
      } else {
        return std::make_tuple(tree.buffer + idx, false);
      }
      goto Lit;
    } else {
      T *res = make(tree.buffer, idx, std::forward<V>(v));
      return std::make_tuple(res, true);
    }
  }

  return std::make_tuple(nullptr, false);
} // bst::insert()

template <typename T, typename C, typename P, typename K>
const T *
find(const StaticTree<T, C, P> &tree, const K &search) noexcept {
  using namespace impl::StaticTree;

  std::size_t idx = 0;
Lit:
  if (idx < tree.capacity) {
    constexpr P present;
    if (present(tree.buffer[idx])) {
      constexpr C cmp;
      if (cmp(search, tree.buffer[idx])) {
        idx = right_child(idx);
      } else if (cmp(tree.buffer[idx], search)) {
        idx = left_child(idx);
      } else {
        return tree.buffer + idx;
      }
      goto Lit;
    }
  }
  return nullptr;
}

template <typename T, typename C, typename P, typename K>
const T *
find(StaticTree<T, C, P> &tree, const K &key) noexcept {
  const StaticTree<T, C, P> &c_tree = tree;
  return find(c_tree, key);
}

template <typename T, typename C, typename P>
bool
verify(StaticTree<T, C, P> &) noexcept {
  return true;
}

template <typename T, typename C, typename P>
void
dump(const StaticTree<T, C, P> &, const std::string &) noexcept {
}

} // namespace bst

#endif
