#ifndef SP_UTIL_MAP_SET_H
#define SP_UTIL_MAP_SET_H

#include <hash/util.h>
#include <limits>
#include <sstream> //debug
#include <tree/avl2.h>
// #include <tree/bst.h>
#include <tree/bst_extra.h>
// #include <tree/red-black.h>
// #include <list/SkipList.h>

// TODO mix of hash to avoid identity 1 -> 1 hash problem
// TODO remove(set,key)
// TODO dynamic node size, not template size look at sparse bitset?

// http://hg.openjdk.java.net/jdk7/jdk7/jdk/file/9b8c96f96a0f/src/share/classes/java/util/HashMap.java
/**
 * Applies a supplemental hash function to a given hashCode, which
 * defends against poor quality hash functions.  This is critical
 * because HashMap uses power-of-two length hash tables, that
 * otherwise encounter collisions for hashCodes that do not differ
 * in lower bits. Note: Null keys always map to hash 0, thus index 0.
 */
/*
 * static int hash(int h) {
 *     // This function ensures that hashCodes that differ only by
 *     // constant multiples at each bit position have a bounded
 *     // number of collisions (approximately 8 at default load factor).
 *     h ^= (h >>> 20) ^ (h >>> 12);
 *     return h ^ (h >>> 7) ^ (h >>> 4);
 * }
 */
namespace sp {
namespace impl {
//=====================================
template <typename T>
struct HSBucket {
  using st = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  HSBucket<T> *next;
  st value;
  bool present;

  HSBucket() noexcept;
  ~HSBucket() noexcept;
};

//=====================================
struct HashKey;

//=====================================
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

  operator std::string() const noexcept;
};

//=====================================
struct HashKey {
  const std::size_t hash;

  HashKey(std::size_t h) noexcept;

  template <typename T>
  bool
  operator>(const HSNode<T> &o) const noexcept;
};

} // namespace impl

//=====================================
template <typename T, sp::Hasher<T>>
struct HashSet {

  // binary::Tree<impl::HSNode<T>> tree;
  avl2::Tree<impl::HSNode<T>> tree;

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
namespace rec {
template <typename T, sp::Hasher<T> h>
bool
verify(const HashSet<T, h> &) noexcept;

//=====================================
template <typename T, sp::Hasher<T> h>
std::size_t
length(const HashSet<T, h> &) noexcept;

} // namespace rec

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
  const bool res = (start + length) > o.hash;
  if (o.hash == 256) {
    // printf("HSNode[%zu,%zu]>HashKey[%zu] in_range = %s gt=%s\n", start,
    // length,
    //        o.hash //
    //        ,
    //        in_range(*this, o) ? "true" : "false", //
    //        res ? "true" : "false");
  }

  if (in_range(*this, o)) {
    return false;
  }

  if (res) {
    return true;
  }

  return false;
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator>(const HSNode<T> &o) const noexcept {
  const bool res = (start + length) > (o.start + o.length);
  // printf("HSNode(start[%zu]+length[%zu]: %zu) > "
  //        "HSNode(o.start[%zu]+o.length[%zu]: %zu): "
  //        "%s\n",
  //        start, length, start + length,         //
  //        o.start, o.length, o.start + o.length, //
  //        res ? "true" : "false");               //
  return res;
}

template <typename T, std::size_t c>
bool
HSNode<T, c>::operator<(const HSNode<T> &o) const noexcept {
  // verify(tree)
  return start < o.start;
}

template <typename T, std::size_t c>
HSNode<T, c>::operator std::string() const noexcept {
  std::stringstream res;
  res << "HSNode[start[" << start << "],length[" << length << "]]";
  return res.str();
}

inline HashKey::HashKey(std::size_t h) noexcept
    : hash(h) {
}

template <typename T>
bool
HashKey::operator>(const HSNode<T> &o) const noexcept {
  const bool res = hash >= (o.start + o.length);
  // printf("HashKey[%zu]>HSNode[%zu,%zu] in_range = %s gt=%s\n", hash,
  //        o.start,
  //        o.length,                              //
  //        in_range(o, *this) ? "true" : "false", //
  //        res ? "true" : "false");
  if (in_range(o, *this)) {
    return false;
  }

  return res;
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
    }

    if (current->next) {
      current = current->next;
      goto Lit;
    }
    current = current->next = new (std::nothrow) HSBucket<T>;

    if (current) {
      assertx(!current->present);

      current->present = true;
      return new (&current->value) T(std::forward<V>(val));
    }
  } else {
    assertx(false);
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
    if (!verify(self.tree)) {
      dump(self.tree);
      assertx(false);
    }

    source.length -= split;
    auto status = emplace(self.tree, start, split);
    if (!verify(self.tree)) {
      dump(self.tree);
      verify(self.tree);
      assertx(false);
    }

    bool created = std::get<1>(status);
    assertx(created);
    HSNode<T, cap> *const result = std::get<0>(status);
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
migrate_list(HSNode<T, cap> &node, HSBucket<T> *const current,
             HSNode<T, cap> &dest) {
  if (current) {

    HSBucket<T> *const next = current->next;
    if (current->present) {

      const HashKey code(hash<T, h>(*current));
      if (!in_range(node, code)) {
        assertx(in_range(dest, code));

        HSBucket<T> &bucket = lookup(dest, code);
        current->next = bucket.next;
        bucket.next = current;

        assertx(node.entries > 0);
        --node.entries;
        ++dest.entries;

        return migrate_list<T, h, cap>(node, next, dest);
      }
    }

    current->next = migrate_list<T, h, cap>(node, next, dest);
    return current;
  }

  return nullptr;
}
} // namespace rec

template <typename T, sp::Hasher<T> h, std::size_t cap>
static void
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
}

template <typename T, sp::Hasher<T> h, std::size_t cap>
static bool
rehash(HashSet<T, h> &self, HSNode<T, cap> &source) noexcept {
  HSNode<T, cap> *const other = split(self, source);
  assertx(verify(self.tree));

  if (other) {
    for (std::size_t i = 0; i < source.capacity; ++i) {
      migrate<T, h, cap>(source, source.buckets[i], *other);
    }

    return true;
  }

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
    assertx(is_empty(tree));

    const std::size_t start = 0;
    const auto length = std::numeric_limits<std::size_t>::max();
    auto res = emplace(tree, start, length);

    node = std::get<0>(res);
    if (node) {
      const bool created = std::get<1>(res);
      assertxs(created, node, created);
    }
  }

  if (node) {
    assertx(find(tree, code) == node);
    assertx(in_range(*node, code));

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
namespace rec {
namespace impl {
template <typename T, std::size_t c, typename F>
static void
for_each(const sp::impl::HSNode<T, c> &node, F f) noexcept {
  for (std::size_t i = 0; i < node.capacity; ++i) {
    const sp::impl::HSBucket<T> &bucket = node.buckets[i];
    f(bucket);
  }
}

template <typename T, typename F>
static void
for_each(const sp::impl::HSBucket<T> &bucket, F f) noexcept {
  const sp::impl::HSBucket<T> *it = &bucket;
  while (it) {
    if (it->present) {
      const T *const value = (T *)&it->value;
      f(*value);
    }
    it = it->next;
  }
}
} // namespace impl

template <typename T, sp::Hasher<T> h>
bool
verify(const sp::HashSet<T, h> &self) noexcept {
  using namespace impl;

  assertx(verify(self.tree));

  binary::rec::inorder(self.tree, [](const auto &node) {
    for_each(node, [&node](const auto &buckets) {
      for_each(buckets, [&node](const T &value) {
        const sp::impl::HashKey code(h(value));

        assertxs(sp::impl::in_range(node, code), code.hash, node.start,
                 node.length);
      });
    });
    /**/
  });
  return true;
}

//=====================================
template <typename T, sp::Hasher<T> h>
std::size_t
length(const HashSet<T, h> &self) noexcept {
  using namespace impl;

  std::size_t length = 0;
  binary::rec::inorder(self.tree, [&length](const auto &node) {
    for_each(node, [&node, &length](const auto &buckets) {
      for_each(buckets, [&node, &length](const T &) {
        /**/
        ++length;
      });
    });
    /**/
  });
  return length;
}

} // namespace rec
//=====================================
} // namespace sp

#endif
