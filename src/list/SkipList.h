#ifndef SP_UTIL_LIST_SKIP_LIST_H
#define SP_UTIL_LIST_SKIP_LIST_H

#include <prng/xorshift.h>
#include <tuple>
#include <util/assert.h>
#include <util/comparator.h>
#include <utility>

/*
 * TODO refactor to use SkipListNode as a header this will reduce the amount of
 * special cases.
 */

/*Sorted container*/
namespace sp {
namespace impl {
//=====================================
template <typename T, std::size_t levels>
struct SkipListNode;
} // namespace impl

//=====================================
/* Level 0 should be the plain linked list structure for each higher level the
 * amount of nodes should be halved. This is accomplished by the random level
 * function, by counting in base 2 the trailing zeros which should for each
 * round halve the likelihood of being zero. Thus affectively achieves the
 * desired logic.
 */
template <typename T, std::size_t levels, typename Comparator = sp::greater>
struct SkipList {
  using value_type = T;

  impl::SkipListNode<T, levels> *header[levels];
  prng::xorshift32 state;

  SkipList() noexcept;
  ~SkipList() noexcept;

  SkipList(const SkipList &) = delete;
  SkipList(const SkipList &&) = delete;

  SkipList &
  operator=(const SkipList &) = delete;
  SkipList &
  operator=(const SkipList &&) = delete;

  static_assert(levels > 0, "gt 0");
  static_assert(levels < 32, "lt 32");
};

//=====================================
template <typename T, std::size_t l, typename C, typename... Arg>
T *
emplace(SkipList<T, l, C> &, Arg &&...) noexcept;

//=====================================
template <typename T, std::size_t l, typename C, typename V>
T *
insert(SkipList<T, l, C> &, V &&) noexcept;

//=====================================
template <typename T, std::size_t l, typename C, typename V>
std::tuple<T *, bool>
insert_unique(SkipList<T, l, C> &, V &&) noexcept;

// template <typename T, std::size_t l, typename C, typename Arg...>
// std::tuple<T *, bool>
// emplace_unique(SkipList<T, l, C> &, Arg &&...) noexcept;

//=====================================
template <typename T, std::size_t l, typename C, typename K>
T *
find(SkipList<T, l, C> &, const K &) noexcept;

template <typename T, std::size_t l, typename C, typename K>
const T *
find(const SkipList<T, l, C> &, const K &) noexcept;

//=====================================
template <typename T, std::size_t l, typename C, typename K>
bool
take(SkipList<T, l, C> &, const K &, T &) noexcept;

//=====================================
template <typename T, std::size_t l, typename C, typename K>
bool
remove(SkipList<T, l, C> &, const K &) noexcept;

//=====================================
template <typename T, std::size_t l, typename C>
bool
is_empty(const SkipList<T, l, C> &) noexcept;

//=====================================
template <typename T, std::size_t l, typename C>
void
swap(SkipList<T, l, C> &, SkipList<T, l, C> &) noexcept;

//=====================================
template <typename T, std::size_t levels, typename C, typename F>
bool
for_all(const SkipList<T, levels, C> &, F) noexcept;

template <typename T, std::size_t levels, typename C, typename F>
bool
for_all(SkipList<T, levels, C> &, F) noexcept;

//=====================================
template <typename T, std::size_t levels, typename C, typename F>
void
for_each(const SkipList<T, levels, C> &, F) noexcept;

template <typename T, std::size_t levels, typename C, typename F>
void
for_each(SkipList<T, levels, C> &, F) noexcept;

//=====================================
template <std::size_t l, typename C>
void
dump(const SkipList<int, l, C> &) noexcept;

//=====================================
namespace n {
template <typename T, std::size_t levels, typename C>
std::size_t
length(const SkipList<T, levels, C> &) noexcept;
}

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T, std::size_t levels>
struct SkipListNode {
  SkipListNode<T, levels> *next[levels];
  T value;

  template <typename V>
  explicit SkipListNode(V &&) noexcept;

  template <typename... Arg>
  SkipListNode(Arg &&... args) noexcept;

  ~SkipListNode() noexcept;
};

template <typename T, std::size_t levels>
template <typename V>
SkipListNode<T, levels>::SkipListNode(V &&v) noexcept
    : next{nullptr}
    , value{std::forward<V>(v)} {
}

template <typename T, std::size_t levels>
template <typename... Arg>
SkipListNode<T, levels>::SkipListNode(Arg &&... args) noexcept
    : next{nullptr}
    , value{std::forward<Arg>(args)...} {
}

template <typename T, std::size_t levels>
SkipListNode<T, levels>::~SkipListNode() noexcept {
}
} // namespace impl

//=====================================
template <typename T, std::size_t levels, typename C>
SkipList<T, levels, C>::SkipList() noexcept
    : header{nullptr}
    , state{} {
}

template <typename T, std::size_t levels, typename C>
SkipList<T, levels, C>::~SkipList() noexcept {
  auto *it = header[0];
  while (it) {
    auto *const next = it->next[0];

    delete it;
    it = next;
  }
}

namespace impl {
//=====================================
template <typename T, std::size_t levels, typename C, typename K>
static bool
is_equal(const SkipListNode<T, levels> *node, const K &needle) {
  if (node) {
    constexpr C cmp;
    if (!cmp(needle, node->value) && !cmp(node->value, needle)) {
      return true;
    }
  }

  return false;
}

template <typename T, std::size_t levels, typename C, typename K>
static bool
is_next_equal(const SkipListNode<T, levels> *node, const K &needle,
              std::size_t level) {
  if (node) {
    return is_equal<T, levels, C, K>(node->next[level], needle);
  }
  return false;
}
} // namespace impl

//=====================================
namespace impl {
/* Starting from the highest present node in the list header, search downwards
 * until we are on target_level, then horizontally search for needle node
 * predecessor.
 */
template <typename T, std::size_t levels, typename C, typename K>
static SkipListNode<T, levels> *
find_predecessor(SkipList<T, levels, C> &self, std::size_t target_level,
                 const K &needle) noexcept {
  SkipListNode<T, levels> *previous = nullptr;
  SkipListNode<T, levels> *current = nullptr;
  std::size_t level = levels - 1;

Llevel:
  if (!current) {
    current = self.header[level];
    previous = nullptr;
  }

Lhorizontal:
  if (current) {
    constexpr C cmp;
    if (cmp(needle, current->value)) { // needle > it

      if (current->next[level]) {
        previous = current;
        current = current->next[level];
        goto Lhorizontal;
      }
    } else if (cmp(current->value, needle)) { // needle < it
      current = previous;
      previous = nullptr;
    } else {
      current = previous;
      previous = nullptr;
    }
  }

  if (level > target_level) {
    --level;
    goto Llevel;
  }

  return current;
}

template <typename T, std::size_t levels, typename C, typename K>
static SkipListNode<T, levels> *
find_level_predecessor(SkipListNode<T, levels> *start, std::size_t level,
                       const K &needle) noexcept {
  SkipListNode<T, levels> *previous = nullptr;
Lit:
  if (start) {
    constexpr C cmp;

    /* it > needle = return $priv */
    if (cmp(start->value, needle)) {
      return previous;
    } else if (cmp(needle, start->value)) { // it < needle = continue
      previous = start;
      start = start->next[level];
      goto Lit;
    } else { // exact match
      return previous;
    }
  } else {
    return previous;
  }

  return nullptr;
}

/* Randomize the depth of the about-to-inserted node
 */
template <typename T, std::size_t levels, typename C>
static std::size_t
random_level(SkipList<T, levels, C> &self, std::size_t max) {
  prng::xorshift32 &state = self.state;

  /* Generates a random number
   * Bitwise count the amount of leading zeroes <--
   * Limit the count to max
   *
   * For each level of bit compare we get a 50/50 chance that the bit is 2/0.
   * This mirrors the requirement of the SkipList that each level contains
   * half the amount of nodes as its preceding level.
   *
   * ...
   * L2 | 25%
   * L1 | 50%
   * L0 | 100%
   */

  // TODO verify this is correct
  std::uint32_t x = random(state);
  std::size_t level = 0;
  while (((x >>= 1) & 1) != 0) {
    ++level;
  }

  return level % max;
}

template <typename T, std::size_t L, typename C>
static T *
insert_node(SkipList<T, L, C> &self, SkipListNode<T, L> *node) noexcept {
  assertx(node);

  /* Randomly generate what level $node will be inserted into. The level also
   * includes lower levels as well.
   */
  const std::size_t target_level = random_level(self, L);

  /* Search horizontally and vertically for the predecessor to node->value
   * limit the result to target_level inclusive.
   */
  auto *start = find_predecessor(self, target_level, node->value);

  for (std::size_t level = target_level + 1; level-- > 0;) {
    if (!start) {
      /* Start from the beginning of the level since there was NO higher
       * level express-lane
       */
      start = self.header[level];
    }

    /* Search this level for the predecessor to node->value
     */
    auto *pred = find_level_predecessor<T, L, C>(start, level, node->value);
    if (pred) {
      /* We update chain on this level by inserting $node:
       * $pred->self->next
       */
      auto next = pred->next[level];
      pred->next[level] = node;
      node->next[level] = next;

      /* Start next level search from the $pred node on this level
       */
      start = pred;
    } else {
      /* This level is empty, or $node is greater than the first node in chain:
       * thus inserting $node first in level
       */
      auto *next = self.header[level];
      node->next[level] = next;
      self.header[level] = node;

      /* Since we could not find a $pred on this level we have to start
       * from the beginning on the next level
       */
      start = nullptr;
    }
  } // for

  return &node->value;
}
} // namespace impl

template <typename T, std::size_t l, typename C, typename... Arg>
T *
emplace(SkipList<T, l, C> &self, Arg &&... args) noexcept {
  using namespace impl;

  auto node = new (std::nothrow) SkipListNode<T, l>{std::forward<Arg>(args)...};
  if (node) {
    return insert_node(self, node);
  }

  return nullptr;
}

//=====================================
template <typename T, std::size_t L, typename C, typename V>
T *
insert(SkipList<T, L, C> &self, V &&v) noexcept {
  using namespace impl;

  auto node = new (std::nothrow) SkipListNode<T, L>{std::forward<V>(v)};
  if (node) {
    return insert_node(self, node);
  }

  return nullptr;
}

//=====================================
template <typename T, std::size_t L, typename C, typename V>
std::tuple<T *, bool>
insert_unique(SkipList<T, L, C> &self, V &&val) noexcept {
  using namespace impl;

  SkipListNode<T, L> *cache[L] = {nullptr};

  SkipListNode<T, L> *start = nullptr;
  for (std::size_t level = L; level-- > 0;) {
    if (!start) {
      start = self.header[level];
    }

    start = cache[level] = find_level_predecessor<T, L, C>(start, level, val);
    if (cache[level]) {
      if (is_next_equal<T, L, C, V>(cache[level], val, level)) {
        return std::make_tuple(&cache[level]->next[level]->value, false);
      }
    } else {
      if (is_equal<T, L, C, V>(self.header[level], val)) {
        return std::make_tuple(&self.header[level]->value, false);
      }
    }
  }

  auto node = new (std::nothrow) SkipListNode<T, L>{std::forward<V>(val)};
  if (node) {
    const std::size_t target_level = random_level(self, L);
    for (std::size_t level = target_level + 1; level-- > 0;) {
      if (cache[level]) {
        auto next = cache[level]->next[level];
        cache[level]->next[level] = node;
        node->next[level] = next;
      } else {
        auto *next = self.header[level];
        node->next[level] = next;
        self.header[level] = node;

        start = nullptr;
      }
    }
    return std::make_tuple(&node->value, true);
  }

  return std::make_tuple(nullptr, false);
}

//=====================================
// template <typename T, std::size_t l, typename C, typename Arg...>
// std::tuple<T *, bool>
// emplace_unique(SkipList<T, l, C> &, Arg &&...) noexcept {
//   using namespace impl::SkipList;
//
//   auto s = new (std::nothrow) SkipListNode<T, L>{std::forward<Arg>(args)...};
//   if (s) {
//     return impl::insert_unique(self, s);
//   }
//
//   return nullptr;
// }

//=====================================
namespace impl {
template <typename T, std::size_t levels, typename C>
static std::size_t
first_highest(const SkipList<T, levels, C> &self, std::size_t limit) {
  for (std::size_t level = levels; level-- > 0;) {
    if (level == limit) {
      return limit;
    }

    if (self.header[level]) {
      return level;
    }
  }

  return levels;
}

template <typename T, std::size_t levels, typename C, typename K>
static const SkipListNode<T, levels> *
find_node(const SkipList<T, levels, C> &self, const K &needle) noexcept {

  std::size_t level = first_highest(self, levels);
  if (level < levels) {
    SkipListNode<T, levels> *current = self.header[level];
    SkipListNode<T, levels> *previous = nullptr;

  Lit:
    if (current) {
      constexpr C cmp;
      if (cmp(needle, current->value)) { // needle > it

        previous = current;
        current = current->next[level];
        goto Lit;
      } else if (cmp(current->value, needle)) { // needle < it

        if (level > 0) {
          --level;
          if (!previous) {
            current = self.header[level];
          } else {
            current = previous;
          }
          previous = nullptr;

          goto Lit;
        }
      } else { // needle == it

        return current;
      }
    } else {
      if (level > 0) { // TODO?
        current = previous;
        --level;
        goto Lit;
      }
    }
  }

  return nullptr;
}
} // namespace impl

template <typename T, std::size_t levels, typename Comparator, typename K>
const T *
find(const SkipList<T, levels, Comparator> &self, const K &needle) noexcept {
  using namespace impl;

  auto node = find_node(self, needle);
  if (node) {
    return &node->value;
  }

  return nullptr;
} // sp::find()

template <typename T, std::size_t l, typename C, typename K>
T *
find(SkipList<T, l, C> &self, const K &needle) noexcept {
  const auto &c_self = self;
  return (T *)find(c_self, needle);
}

//=====================================
namespace impl {
template <typename T, std::size_t L, typename C, typename K>
static SkipListNode<T, L> *
remove_node(SkipList<T, L, C> &self, const K &needle) noexcept {
  SkipListNode<T, L> *result = nullptr;
  SkipListNode<T, L> *current = nullptr;

  for (std::size_t level = L; level-- > 0;) {
    if (!current) {
      current = self.header[level];
      if (is_equal<T, L, C, K>(current, needle)) {
        result = current;
        self.header[level] = current->next[level];
        // TODO done on this leve, go to next level
      }
    }

    /*
     * Search this level for the closest predecessor to needle
     */
    current = find_level_predecessor<T, L, C, K>(current, level, needle);
    if (current) {
      auto *next = current->next[level];
      if (is_equal<T, L, C, K>(next, needle)) {
        result = next;

        // unlink result
        current->next[level] = next->next[level];
      }
    }
  }

  return result;
}
} // namespace impl

template <typename T, std::size_t l, typename C, typename K>
bool
take(SkipList<T, l, C> &self, const K &needle, T &out) noexcept {
  using namespace impl;

  auto *result = remove_node(self, needle);
  if (result) {
    using sp::swap;
    using std::swap;
    swap(result->value, out);

    delete result;
    return true;
  }

  return false;
}

//=====================================
template <typename T, std::size_t L, typename C, typename K>
bool
remove(SkipList<T, L, C> &self, const K &needle) noexcept {
  using namespace impl;

  auto *result = remove_node(self, needle);
  if (result) {
    delete result;
  }

  return result != nullptr;
} // sp::remove()

//=====================================
template <typename T, std::size_t levels, typename C>
bool
is_empty(const SkipList<T, levels, C> &self) noexcept {
  return self.header[0] == nullptr;
}

//=====================================
template <typename T, std::size_t levels, typename C>
void
swap(SkipList<T, levels, C> &f, SkipList<T, levels, C> &s) noexcept {
  using std::swap;

  for (std::size_t i = 0; i < levels; ++i) {
    swap(f.header[i], s.header[i]);
  }

  swap(f.state, s.state);
}

//=====================================
template <typename T, std::size_t levels, typename C, typename F>
bool
for_all(const SkipList<T, levels, C> &l, F f) noexcept {
  auto *it = l.header[0];
  while (it) {
    const auto &current = it->value;

    if (!f(current)) {
      return false;
    }

    it = it->next[0];
  }

  return true;
}

template <typename T, std::size_t levels, typename C, typename F>
bool
for_all(SkipList<T, levels, C> &l, F f) noexcept {
  auto *it = l.header[0];
  while (it) {
    auto &current = it->value;

    if (!f(current)) {
      return false;
    }

    it = it->next[0];
  }

  return true;
}

//=====================================
template <typename T, std::size_t levels, typename C, typename F>
void
for_each(const SkipList<T, levels, C> &l, F f) noexcept {
  auto *it = l.header[0];

  while (it) {
    const auto &current = it->value;
    f(current);

    it = it->next[0];
  }
}

template <typename T, std::size_t levels, typename C, typename F>
void
for_each(SkipList<T, levels, C> &l, F f) noexcept {
  auto *it = l.header[0];

  while (it) {
    auto &current = it->value;
    f(current);

    it = it->next[0];
  }
}

//=====================================
template <std::size_t levels, typename C>
void
dump(const SkipList<int, levels, C> &self) noexcept {
  using namespace impl;

  if (self.header[0] == nullptr) {
    return;
  }

  auto index_of = [&self](const auto *node) {
    const SkipListNode<int, levels> *it = self.header[0];
    std::size_t result = 0;
    while (it) {
      if (node == it) {
        return result;
      }
      it = it->next[0];
      ++result;
    }
    return result;
  };
  auto last_index = [&self] {
    const SkipListNode<int, levels> *it = self.header[0];
    std::size_t result = 0;
  Lit:
    if (it) {
      it = it->next[0];
      result++;
      goto Lit;
    }

    return result;
  };
  constexpr std::size_t b_length = 4;
  auto print_empty = [b_length](std::size_t count) { //
    // printf("count[%zu]\n",count);
    for (std::size_t i = 0; i < count; ++i) {
      printf("+");
      for (std::size_t a = 0; a < b_length; ++a) {
        printf("-");
      }
      printf("-");
    }
    return bool(count > 0);
  };
  auto print_node = [b_length](auto *node, bool first) {
    char buffer[b_length];
    std::memset(buffer, ' ', b_length);
    auto idx = sprintf(buffer, "%d", node->value);
    buffer[idx] = ' ';
    buffer[b_length - 1] = '\0';
    if (first) {
      printf("[%s]-", buffer);
    } else {
      printf(">[%s]", buffer);
    }
  };

  for (std::size_t level = levels; level-- > 0;) {
    std::size_t print_index = 0;
    const SkipListNode<int, levels> *it = self.header[level];
    if (level == 0) {
      printf("BL |");
    } else {
      printf("L%zu |", level);
    }
    bool first = true;
    std::size_t line_length = 0;
    while (it) {
      std::size_t index = index_of(it);
      // ++print_index;//about to print
      bool printed_empty = print_empty(index - print_index);
      if (first) {
        first = !printed_empty;
      }
      print_index = index + 1;
      print_node(it, first);
      first = false;
      it = it->next[level];
      ++line_length;
    }

    std::size_t index = last_index();
    print_empty(index - print_index);
    printf(">| %zu node(s)\n", line_length);
  }
}

namespace n {
//=====================================
template <typename T, std::size_t levels, typename C>
std::size_t
length(const SkipList<T, levels, C> &self) noexcept {
  std::size_t result = 0;
  for_each(self, [&result](const auto &) {
    /**/
    ++result;
  });
  return result;
}

//=====================================
} // namespace n

} // namespace sp

#endif
