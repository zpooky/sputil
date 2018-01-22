#ifndef SP_UTIL_TREE_STATIC2_TREE_H
#define SP_UTIL_TREE_STATIC2_TREE_H

#include <tree/tree.h>
#include <tuple>
#include <util/comparator.h>

namespace bst {
template <typename T, typename Comparator = sp::greater,
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

template <typename C, typename P>
void
dump(const StaticTree<int, C, P> &, const std::string & = "") noexcept;
//==================
namespace impl {
namespace StaticTree {

static inline std::size_t
parent(std::size_t idx) noexcept {
  return (idx - 1) / 2;
}

static inline std::size_t
left_child(std::size_t idx) noexcept {
  return (2 * idx) + 1;
}

static inline std::size_t
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

template <typename T, typename C, typename P>
static bool
is_present(const bst::StaticTree<T, C, P> &tree, std::size_t idx) noexcept {
  if (idx < tree.capacity) {
    constexpr P present;
    return present(tree.buffer[idx]);
  }
  return false;
}

template <typename T, typename C, typename P>
std::size_t
find_min(bst::StaticTree<T, C, P> &tree, std::size_t index) noexcept {
  std::size_t result = index;
Lit:
  if (index < tree.capacity) {
    if (is_present(tree, index)) {
      result = index;
      index = left_child(index);
      goto Lit;
    }
  }
  return result;
}

template <typename T, typename C, typename P>
void
reset(bst::StaticTree<T, C, P> &tree, std::size_t index) noexcept {
  T *ptr = tree.buffer + index;
  ptr->~T();
  new (ptr) T(-1);
}

template <typename T, typename C, typename P>
void
swap(bst::StaticTree<T, C, P> &tree, std::size_t f, std::size_t s) noexcept {
  std::swap(tree.buffer[f], tree.buffer[s]);
}

template <typename T, typename C, typename P>
std::size_t
remove(bst::StaticTree<T, C, P> &tree, std::size_t index,
       bool should_reset) noexcept {
  std::size_t left = left_child(index);
  std::size_t right = right_child(index);
  if (should_reset) {
    reset(tree, index);
  }

  if (is_present(tree, left) && is_present(tree, right)) {
    printf("2children(%d, %d)\n", tree.buffer[left], tree.buffer[right]);
    // two children

    // Replace current with the smallest right child
    std::size_t successor = find_min(tree, right);
    swap(tree, index, successor);

    return successor;
  } else if (!is_present(tree, left) && !is_present(tree, right)) {
    printf("0children(%d)\n", tree.buffer[index]);
    // zero children

    return index;
  } else if (is_present(tree, left)) {
    printf("left_children(%d)\n", tree.buffer[left]);
    // one child

    swap(tree, index, left);
    return remove(tree, left, false);
  } else {
    printf("right_children(%d)\n", tree.buffer[right]);
    // one child

    swap(tree, index, right);
    return remove(tree, right, false);
  }
} // impl::bst::remove()

template <typename T, typename C, typename P, typename K>
std::size_t
find_index(const bst::StaticTree<T, C, P> &tree, const K &search) noexcept {
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
        return idx;
      }
      goto Lit;
    }
  }

  return tree.capacity;
} // impl::StaticTree::find_index()

template <typename C, typename P>
void
dump(const bst::StaticTree<int, C, P> &tree, std::size_t idx,
     std::string prefix, bool isTail = true, const char *ctx = "") noexcept {
  if (is_present(tree, idx)) {
    char name[256] = {0};
    sprintf(name, "%s%d", ctx, tree.buffer[idx]);

    printf("%s%s%s\n", prefix.c_str(), (isTail ? "└── " : "├── "), name);

    const char *ls = (isTail ? "    " : "│   ");
    std::size_t left = left_child(idx);
    std::size_t right = right_child(idx);
    if (is_present(tree, right) && is_present(tree, left)) {
      dump(tree, right, prefix + ls, false, "gt:");
      dump(tree, left, prefix + ls, true, "lt:");
    } else {
      if (is_present(tree, left)) {
        dump(tree, left, prefix + ls, true, "lt:");
      } else if (is_present(tree, right)) {
        dump(tree, right, prefix + ls, true, "gt:");
      }
    }
  }
} // impl::StaticTree::dump()

} // namespace StaticTree
} // namespace impl

template <typename T, typename C, typename P>
StaticTree<T, C, P>::StaticTree(T *b, std::size_t l) noexcept
    : buffer(b)
    , capacity(l) {
}

template <typename T, typename C, typename P>
template <std::size_t SIZE>
StaticTree<T, C, P>::StaticTree(T (&b)[SIZE]) noexcept
    : StaticTree(b, SIZE) {
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
      assert(res);
      return std::make_tuple(res, true);
    }
  }

  return std::make_tuple(nullptr, false);
} // bst::insert()

template <typename T, typename C, typename P, typename K>
const T *
find(const StaticTree<T, C, P> &tree, const K &search) noexcept {
  using namespace impl::StaticTree;

  std::size_t idx = find_index(tree, search);
  if (idx != tree.capacity) {
    return tree.buffer + idx;
  }
  return nullptr;
}

template <typename T, typename C, typename P, typename K>
const T *
find(StaticTree<T, C, P> &tree, const K &key) noexcept {
  const StaticTree<T, C, P> &c_tree = tree;
  return find(c_tree, key);
}

// TODO the recursive balance does not work in remove(tree,idx,bool)
template <typename T, typename C, typename P, typename K>
bool
remove(StaticTree<T, C, P> &tree, const K &search) noexcept {
  using namespace impl::StaticTree;

  std::size_t idx = find_index(tree, search);
  if (idx != tree.capacity) {
    remove(tree, idx, true);
    return true;
  }

  return false;
}

template <typename T, typename C, typename P, typename F>
void
in_order_for_each(StaticTree<T, C, P> &tree, F f) {
  using namespace impl::StaticTree;

  enum class Direction : std::uint8_t { LEFT, RIGHT };
  enum class Traversal : uint8_t { UP, DOWN };

  Direction d[1024]{Direction::LEFT};
  Traversal t = Traversal::UP;

  auto set_direction = [&d](std::size_t lvl, Direction dir) { //
    d[lvl] = dir;
  };

  std::size_t level = 0;
  std::size_t idx(0);

Lit:
  if (idx < tree.capacity) {
    if (t == Traversal::UP) {
      if (d[level] == Direction::LEFT) {
        level++;
        set_direction(level, Direction::LEFT);
        idx = left_child(idx);
      } else {
        level++;
        set_direction(level, Direction::LEFT);
        idx = right_child(idx);
      }
      goto Lit;
    } else /*t == DOWN*/ {
      if (d[level] == Direction::LEFT) {
        // We returned to this level after traversed left, now traverse right

        // Indicate that we have consumed right when returning to this level[0]
        d[level] = Direction::RIGHT;

        //
        f(tree.buffer[idx]);

        //
        t = Traversal::UP;
        ++level;
        set_direction(level, Direction::LEFT);
        idx = right_child(idx);

        goto Lit;
      } else { //[0]
        if (level > 0) {
          idx = parent(idx);
          level--;
          // printf("down[idx[%zu], level[%zu]]\n", std::size_t(idx), level);
          goto Lit;
        }
      }
    }
  } else {
    idx = parent(idx);
    level--;
    t = Traversal::DOWN;
    goto Lit;
  }
}

template <typename T, typename C, typename P>
bool
verify(StaticTree<T, C, P> &) noexcept {
  return true;
}

template <typename C, typename P>
void
dump(const StaticTree<int, C, P> &tree, const std::string &prefix) noexcept {
  using namespace impl::StaticTree;
  dump(tree, 0, prefix);
}

} // namespace bst

#endif
