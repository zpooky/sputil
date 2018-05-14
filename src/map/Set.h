#ifndef SP_UTIL_MAP_SET_H
#define SP_UTIL_MAP_SET_H

#include <hash/util.h>
#include <limits>
#include <sstream>
#include <tree/avl.h>
#include <tree/bst.h>
#include <tree/red-black.h>

namespace sp {
namespace impl {
template <typename T>
struct HSBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  HSBucket<T> *next;
  st value;
  bool present;

  HSBucket() noexcept;
  ~HSBucket() noexcept;
};

struct HashKey;

template <typename T, std::size_t c = 256>
struct HSNode {
  static constexpr std::size_t capacity = c;
  std::size_t entries;

  const std::size_t start;
  std::size_t length;

  HSBucket<T> buckets[c];

  HSNode(std::size_t, std::size_t) noexcept;

  bool
  operator>(const HashKey &) const noexcept;

  bool
  operator>(const HSNode<T> &) const noexcept;

  bool
  operator<(const HSNode<T> &) const noexcept;

  operator std::string() const noexcept {
    std::stringstream res;
    res << "HSNode[start[" << start << "],length[" << length << "]]";
    return res.str();
  }
};

struct HashKey {
  const std::size_t hash;

  HashKey(std::size_t h) noexcept
      : hash(h) {
  }

  template <typename T>
  bool
  operator>(const HSNode<T> &o) const noexcept {
    // printf("hash[%zu] > o[o.start[%zu+o.length[%zu]: %zu]\n", hash, o.start,
    //        o.length, o.start + o.length);
    return hash > (o.start + o.length);
  }
};

} // namespace impl

template <typename T, sp::Hasher<T>>
struct HashSet {

  binary::Tree<impl::HSNode<T>> tree;

  HashSet() noexcept;
};

//=====================================
template <typename T, sp::Hasher<T> h, typename V>
T *
insert(HashSet<T, h> &, V &&) noexcept;

//=====================================
// template <typename T, sp::Hasher<T> h, typename V>
// bool
// contains(const HashSet<T, h> &, const V &) noexcept;
//=====================================
template <typename T, sp::Hasher<T> h, typename V>
const T *
lookup(const HashSet<T, h> &, const V &) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
HSBucket<T>::HSBucket() noexcept
    : next{nullptr}
    , value()
    , present(false) {
}

template <typename T>
HSBucket<T>::~HSBucket() noexcept {
  if (present) {
    present = false;
    T *const v = (T *)&value;
    v->~T();
  }
  if (next) {
    // rec
    delete next;
    next = nullptr;
  }
}

//=====================================
template <typename T, std::size_t c>
HSNode<T, c>::HSNode(std::size_t strt, std::size_t len) noexcept
    : entries(0)
    , start(strt)
    , length{len}
    , buckets{} {
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator>(const HashKey &o) const noexcept {
  if (in_range(*this, o)) {
    return false;
  }

  const bool res = (start + length) >= o.hash;
  printf("(start[%zu] + length[%zu]) >= o.hash[%zu] == %s\n", //
         start, length, o.hash, res ? "true" : "false");
  if (res) {
    return true;
  }

  return false;

  // const bool res = (start) > o.hash;
  // printf("(start[%zu]+length[%zu]: %zu) > o.hash[%zu]: "
  //        "%s\n",
  //        start, length, start + length, //
  //        o.hash,                        //
  //        res ? "true" : "false");

  // return res;
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator>(const HSNode<T> &o) const noexcept {
  // bool res = start > o.start;
  // printf("HSNode::>(start[%zu] > o.start[%zu]): %s\n", start, o.start,
  //        res ? "true" : "false");
  const bool res = (start + length) > (o.start + o.length);
  printf("HSNode::>(start[%zu]+length[%zu]: %zu) > "
         "(o.start[%zu]+o.length[%zu]: %zu): "
         "%s\n",
         start, length, start + length,         //
         o.start, o.length, o.start + o.length, //
         res ? "true" : "false");               //
  return res;
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator<(const HSNode<T> &o) const noexcept {
  // verify(tree)
  return start < o.start;
}

} // namespace impl
//=====================================
template <typename T, sp::Hasher<T> h>
HashSet<T, h>::HashSet() noexcept
    : tree() {
}

//=====================================
namespace impl {

// #<{(|*
//  * Returns a power of two size for the given target capacity.
//  |)}>#
// static final int tableSizeFor(int cap) {
//     int n = cap - 1;
//     n |= n >>> 1;
//     n |= n >>> 2;
//     n |= n >>> 4;
//     n |= n >>> 8;
//     n |= n >>> 16;
//     return (n < 0) ? 1 : (n >= MAXIMUM_CAPACITY) ? MAXIMUM_CAPACITY : n + 1;
// }
template <typename T, typename V>
static T *
insert(HSBucket<T> &node, V &&val) noexcept {
  HSBucket<T> *current = &node;
Lit:
  if (current) {
    if (current->present) {
      const T *const value = (T *)&current->value;
      if (*value == val) {
        // already present
        return nullptr;
      }

      if (current->next) {
        current = current->next;
        goto Lit;
      }

      current = current->next = new (std::nothrow) HSBucket<T>;
    }

    if (current) {
      current->present = true;
      return new (&current->value) T(std::forward<V>(val));
    }
  }

  return nullptr;
}

static inline std::size_t
index_of(const HashKey &code, std::size_t length) noexcept {
  const std::size_t res(code.hash & (length - 1));
  const std::size_t cmp(code.hash % length);
  assertxs(res < length, res, length);
  assertxs(cmp == res, code.hash, length, cmp, res);
  return res;
}

template <typename T, std::size_t cap>
static const HSBucket<T> &
lookup(const HSNode<T, cap> &node, const HashKey &code) noexcept {
  const std::size_t index = index_of(code, node.capacity);
  return node.buckets[index];
}

template <typename T, sp::Hasher<T> hash, std::size_t cap>
static HSNode<T, cap> *
split(HashSet<T, hash> &self, HSNode<T, cap> &source) noexcept {
  const std::size_t split(source.length / std::size_t(2));

  if (split >= source.capacity) {
    const std::size_t start(source.start + split);
    const std::size_t before = source.length;
    source.length -= split;
    auto status = emplace(self.tree, start, split);
    if (!verify(self.tree)) { // TODO debug
      dump(self.tree);
      assertx(false);
    }
    // printf("emplace(self.tree, start[%zu], split[%zu])\n", start, split);

    bool created = std::get<1>(status);
    assertx(created);
    HSNode<T, cap> *const result = std::get<0>(status);
    // assertx(result != nullptr);
    if (result) {
      assertx(result->start == start);
      assertx(result->length == split);
      // printf("Before[%zu, %zu]\n", source.start, source.length);

      // printf("SPlit[%zu, %zu]\n", source.start, source.length);
      // printf("SplitOther[%zu, %zu]\n", result->start, result->length);
      return result;
    } else {
      source.length = before;
    }
  }
  assertx(false); // TODO remove

  return nullptr;
}

template <typename T, sp::Hasher<T> h>
static std::size_t
hash(const HSBucket<T> &subject) noexcept {
  const T *const value = (T *)&subject.value;
  return h(*value);
}

template <typename T, std::size_t cap>
static bool
in_range(const HSNode<T, cap> &node, const HashKey &h) noexcept {
  return (h.hash >= node.start) && h.hash < (node.start + node.length);
}

namespace rec {
template <typename T, sp::Hasher<T> h, std::size_t cap>
HSBucket<T> *
migrate_list(HSNode<T, cap> &node, HSBucket<T> *current, HSNode<T, cap> &dest) {
  // TODO this is recursive
  if (current) {
    if (current->present) {
      const HashKey code(hash<T, h>(*current));
      if (!in_range(node, code)) {
        assertx(in_range(dest, code));
        HSBucket<T> *const next = current->next;

        HSBucket<T> &bucket = lookup(dest, code);
        current->next = bucket.next;
        bucket.next = current;

        assertx(node.entries > 0);
        --node.entries;
        ++dest.entries;

        return migrate_list<T, h, cap>(node, next, dest);
      }
    }

    current->next = migrate_list<T, h, cap>(node, current->next, dest);
    return current;
  }

  return nullptr;
}
} // namespace rec

template <typename T, sp::Hasher<T> h, std::size_t cap>
static bool
migrate(HSNode<T, cap> &node, HSBucket<T> &source,
        HSNode<T, cap> &dest) noexcept {
  if (source.present) {
    const HashKey code(hash<T, h>(source));
    if (!in_range(node, code)) {
      assertx(in_range(dest, code));

      HSBucket<T> &bucket = lookup(dest, code);
      T *const value = (T *)&source.value;
      T *const result = insert(bucket, std::move(*value));
      if (result) {
        assertx(node.entries > 0);
        --node.entries;
        ++dest.entries;
      }
      value->~T();
      source.present = false;
    }
  }

  source.next = rec::migrate_list<T, h, cap>(node, source.next, dest);

  return true;
}

template <typename T, sp::Hasher<T> h, std::size_t cap>
static bool
rehash(HashSet<T, h> &self, HSNode<T, cap> &source) noexcept {
  HSNode<T, cap> *const other = split(self, source);
  if (other) {
    for (std::size_t i = 0; i < source.capacity; ++i) {
      migrate<T, h, cap>(source, source.buckets[i], *other);
    }

    return true;
  }
  assertx(false); // TODO remove

  return false;
} // impl::rehash()

template <typename T, std::size_t cap>
static HSBucket<T> &
lookup(HSNode<T, cap> &node, const HashKey &code) noexcept {
  assertx(in_range(node, code));

  const auto &c_node = node;
  return (HSBucket<T> &)lookup(c_node, code);
}

template <typename T, std::size_t cap, typename V>
static T *
insert(HSNode<T, cap> &node, const HashKey &code, V &&val) noexcept {
  assertx(in_range(node, code));

  HSBucket<T> &bucket = lookup(node, code);
  T *const result = insert(bucket, std::forward<V>(val));
  if (result) {
    ++node.entries;
  }
  return result;
}

} // namespace impl

template <typename T, sp::Hasher<T> hash, typename V>
T *
insert(HashSet<T, hash> &self, V &&val) noexcept {
  using namespace impl;

  auto &tree = self.tree;
  const HashKey code(hash(val));

  HSNode<T> *node = find(tree, code);
  if (node == nullptr) {
    // should only get here on first invocation
    const std::size_t start = 0;
    const auto length = std::numeric_limits<std::size_t>::max();
    auto res = emplace(tree, start, length);

    node = std::get<0>(res);
    assertx(in_range(*node, code)); // TODO only for debug
    assertxs(std::get<1>(res), node, std::get<1>(res));

    // assertx(find(tree, *node) == node); // TODO only for debug
    if (!find(tree, code)) { // TODO only for debug
      dump(tree);
      assertx(false);
    }
  }

  if (node) {
    if (!in_range(*node, code)) {
      dump(tree);
    }
    T *const result = impl::insert(*node, code, std::forward<V>(val));
    if (result) {
      // TODO resize factor
      if (node->entries >= node->capacity) {
        impl::rehash(self, *node);
      }
    }
    return result;
  }

  return nullptr;
}

//=====================================
namespace impl {
template <typename T, typename V>
static const T *
lookup(const HSBucket<T> &node, const V &needle) noexcept {

  const HSBucket<T> *current = &node;
Lit:
  if (current) {
    if (current->present) {
      const T *const value = (T *)&current->value;
      if (*value == needle) {
        return value;
      }
    }
    current = current->next;
    goto Lit;
  }
  return nullptr;
}

template <typename T, std::size_t c, typename V>
static const T *
lookup(const HSNode<T, c> &node, const HashKey &code,
       const V &needle) noexcept {
  assertx(in_range(node, code));

  const HSBucket<T> &bucket = lookup(node, code);
  return lookup(bucket, needle);
}
} // namespace impl

template <typename T, sp::Hasher<T> hash, typename V>
const T *
lookup(const HashSet<T, hash> &self, const V &needle) noexcept {
  using namespace impl;

  const auto &tree = self.tree;
  const HashKey code(hash(needle));

  const HSNode<T> *node = find(tree, code);
  if (node) {
    assertx(in_range(*node, code));

    return impl::lookup(*node, code, needle);
  }

  return nullptr;
}

//=====================================
} // namespace sp

#endif
