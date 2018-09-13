#ifndef SP_UTIL_MAP_HASH_SET_TREE_H
#define SP_UTIL_MAP_HASH_SET_TREE_H

#include <hash/standard.h>
#include <hash/util.h>
#include <limits>
#include <sstream> //debug
#include <tree/avl.h>
#include <type_traits>
// #include <tree/bst.h>
#include <tree/bst_extra.h>
// #include <tree/red-black.h>
// #include <list/SkipList.h>

// TODO mix of hash to avoid identity 1 -> 1 hash problem
// TODO remove(set,key)
// TODO dynamic node size, not template size look at sparse bitset?
// TODO load factor?
//

// XXX if trivialy copyable we can ignore dtor?

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
/*
 * An HashSet implementation using separate chaining collision handling
 * strategy. Meaning on hash collision we place the new entry in a linkedlist
 * pointed to by the bucket.
 *
 * Buckets are split into nodes, which is a group of buckets. Each nodes are
 * responsible for handling a range of hashes. Nodes are placed in an binary
 * search tree and can are accessed by searching for them using a hash code.
 *
 * When a node element count exceed a threshold the node is split in two by
 * dividing the hash range responsibility in two and creating a new node
 * responsible for the second half of the divided hash range. The rehash
 * operation of the original node is performed by for each element in node,
 * calculate its hash value, determine if the hash falls in the domain of the
 * original or the new nodes hash range, if element is supposed to be in the
 * new node; then move it there. The new node is then inserted the node tree.
 */
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

  HashKey(std::size_t) noexcept;

  bool
  operator==(const HashKey &) const noexcept;

  bool operator==(std::size_t) const noexcept;

  template <typename T>
  bool
  operator>(const HSNode<T> &) const noexcept;
};

} // namespace impl

//=====================================
template <typename T, typename Hash = sp::Hasher<T>, typename Eq = sp::Equality>
struct HashSet {

  // binary::Tree<impl::HSNode<T>> tree;
  avl::Tree<impl::HSNode<T>> tree;

  HashSet() noexcept;
  HashSet(const HashSet<T, Hash, Eq> &) = delete;
  HashSet(const HashSet<T, Hash, Eq> &&) = delete;

  ~HashSet() noexcept;
};

//=====================================
/* returns null on duplicate
 */
template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSet<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSet<T, H, Eq> &, V &&) noexcept;

//=====================================
namespace impl {
template <typename T, typename H, typename Eq>
const T *
lookup(const HashSet<T, H, Eq> &, const HashKey &code) noexcept;
}

template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSet<T, H, Eq> &, const V &) noexcept;

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSet<T, H, Eq> &, const V &) noexcept;

//=====================================
template <typename T, typename H, typename Eq>
const T *
lookup_default(const HashSet<T, H, Eq> &, const T &needle,
               const T &def) noexcept;

template <typename T, typename H, typename Eq>
T *
lookup_default(HashSet<T, H, Eq> &, const T &needle, T &def) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
lookup_insert(HashSet<T, H, Eq> &, V &&) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V, typename Compute>
T *
lookup_compute(HashSet<T, H, Eq> &, const V &needle, Compute) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
contains(const HashSet<T, H, Eq> &, const V &) noexcept;

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSet<T, H, Eq> &, const V &) noexcept;

//=====================================
namespace rec {
template <typename T, typename H, typename Eq>
bool
verify(const HashSet<T, H, Eq> &) noexcept;

//=====================================
template <typename T, typename H, typename Eq>
std::size_t
length(const HashSet<T, H, Eq> &) noexcept;

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
  // if (o.hash == 256) {
  // printf("HSNode[%zu,%zu]>HashKey[%zu] in_range = %s gt=%s\n", start,
  // length,
  //        o.hash //
  //        ,
  //        in_range(*this, o) ? "true" : "false", //
  //        res ? "true" : "false");
  // }

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

  constexpr auto mx = std::numeric_limits<std::size_t>::max();
  assertxs(h != mx, h, mx);
}

inline bool
HashKey::operator==(std::size_t o) const noexcept {
  return this->hash == o;
}

inline bool
HashKey::operator==(const HashKey &o) const noexcept {
  return this->operator==(o.hash);
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
template <typename T, typename H, typename Eq>
HashSet<T, H, Eq>::HashSet() noexcept
    : tree() {
}

template <typename T, typename H, typename Eq>
HashSet<T, H, Eq>::~HashSet() noexcept {
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
template <typename T, typename V, typename Eq>
static bool
bucket_remove(HSBucket<T> &node, const V &needle, Eq eq) noexcept {
  HSBucket<T> *current = &node;
Lit:
  if (current) {
    if (current->present) {
      T *const existing = (T *)&current->value;
      if (eq(*existing, needle)) {
        current->present = false;
        existing->~T();

        return true;
      }
    }

    current = current->next;
    goto Lit;
  }

  return false;
}

template <typename T, typename V, typename Fact, typename Dup, typename Eq>
static T *
bucket_gen_ins(HSBucket<T> &node, const V &in, Fact f, Dup d, Eq eq) noexcept {
  HSBucket<T> *current = &node;
Lit:
  if (current) {
    if (current->present) {
      const T *const existing = (const T *)&current->value;
      if (eq(*existing, in)) {
        /* Already present */
        return d(*current);
      }
    }

    if (current->next) {
      current = current->next;
      goto Lit;
    }

    if (current->present) {
      current = current->next = new (std::nothrow) HSBucket<T>;
    }

    if (current) {
      assertx(!current->present);
      current->present = true;
      f(*current, in);
      return (T *)&current->value;
    }

  } else {
    assertx(false);
  }

  return nullptr;
}

template <typename T, typename V, typename Eq>
static T *
bucket_insert(HSBucket<T> &node, V &&val, bool &inserted, Eq eq) noexcept {
  auto factory = [&val](HSBucket<T> &current, const auto &) {
    return new (&current.value) T(std::forward<V>(val));
  };

  auto on_duplicate = [](HSBucket<T> &) {
    /* $value is already present */
    return nullptr;
  };

  T *const result = bucket_gen_ins(node, val, factory, on_duplicate, eq);
  inserted = result != nullptr;

  return result;
}

template <typename T, typename V, typename Eq>
static T *
bucket_lookup_ins(HSBucket<T> &node, V &&val, bool &inserted, Eq eq) noexcept {
  auto factory = [&val](HSBucket<T> &current, const auto &) {
    return new (&current.value) T(std::forward<V>(val));
  };

  bool existing = false;
  auto on_duplicate = [&existing](HSBucket<T> &current) {
    /* $value is already present, return it */
    existing = true;
    return (T *)&current.value;
  };

  T *const result = bucket_gen_ins(node, val, factory, on_duplicate, eq);
  inserted = !existing && result != nullptr;

  return result;
}

template <typename T, typename V, typename Eq>
static T *
bucket_upsert(HSBucket<T> &node, V &&val, bool &inserted, Eq eq) noexcept {
  auto factory = [&val](HSBucket<T> &current, const auto &) {
    return new (&current.value) T(std::forward<V>(val));
  };

  bool updated = false;
  auto on_duplicate = [&updated, &val](HSBucket<T> &current) {
    /* $value is already present, insert update */
    assertx(current.present);

    updated = true;

    T *const existing = (T *)&current.value;
    existing->~T();
    return new (existing) T(std::forward<V>(val));
  };

  T *const result = bucket_gen_ins(node, val, factory, on_duplicate, eq);
  inserted = result != nullptr && !updated;

  return result;
}

template <typename T, typename V, typename Factory, typename Eq>
static T *
bucket_lookup_compute(HSBucket<T> &node, const V &needle, Factory f,
                      bool &inserted, Eq eq) noexcept {
  bool existing = false;
  auto on_duplicate = [&](HSBucket<T> &current) {
    /* $value is already present, return it */
    existing = true;
    return (T *)&current.value;
  };

  T *const result = bucket_gen_ins(node, needle, f, on_duplicate, eq);
  inserted = !existing && result != nullptr;

  return result;
}

static inline std::size_t
index_of(const HashKey &code, std::size_t length) noexcept {
  const std::size_t res = code.hash & (length - 1);
  const std::size_t cmp = code.hash % length;
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

template <typename T, typename Hash, typename Eq, std::size_t cap>
static HSNode<T, cap> *
split(HashSet<T, Hash, Eq> &self, HSNode<T, cap> &source) noexcept {
  const std::size_t split(source.length / std::size_t(2));

  if (split >= source.capacity) {
    const std::size_t start(source.start + split);
    const std::size_t before = source.length;
    assertx_f({
      if (!verify(self.tree)) {
        dump(self.tree);
        assertx(false);
      }
    });

    source.length -= split;
    auto status = emplace(self.tree, start, split);
    assertx_f({
      if (!verify(self.tree)) {
        dump(self.tree);
        verify(self.tree);
        assertx(false);
      }
    });

    bool created = std::get<1>(status);
    assertx(created);
    HSNode<T, cap> *const result = std::get<0>(status);
    if (result) {
      assertxs(result->start == start, result->start, start);
      assertxs(result->length == split, result->length, split);
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

template <typename T, typename H>
static std::size_t
hash(const HSBucket<T> &subject) noexcept {
  assertx(subject.present);

  const T *const value = (const T *)&subject.value;

  H h;
  return h(*value);
}

template <typename T, std::size_t cap>
static bool
in_range(const HSNode<T, cap> &node, const HashKey &h) noexcept {
  return (h.hash >= node.start) && h.hash < (node.start + node.length);
}

namespace rec {
template <typename T, typename H, std::size_t cap>
HSBucket<T> *
migrate_list(HSNode<T, cap> &node, HSBucket<T> *const current,
             HSNode<T, cap> &dest) {
  if (current) {

    HSBucket<T> *const next = current->next;
    if (current->present) {

      const HashKey code(hash<T, H>(*current));
      if (!in_range(node, code)) {
        assertxs(in_range(dest, code), dest.start, dest.length, code.hash);

        HSBucket<T> &bucket = lookup(dest, code);
        current->next = bucket.next;
        bucket.next = current;

        assertx(node.entries > 0);
        --node.entries;
        ++dest.entries;

        return migrate_list<T, H, cap>(node, next, dest);
      }
    }

    current->next = migrate_list<T, H, cap>(node, next, dest);
    return current;
  }

  return nullptr;
}
} // namespace rec

template <typename T, typename H, typename Eq, std::size_t cap>
static void
migrate(HSNode<T, cap> &node, HSBucket<T> &source,
        HSNode<T, cap> &dest) noexcept {
  if (source.present) {
    const HashKey code(hash<T, H>(source));
    if (!in_range(node, code)) {
      assertx(in_range(dest, code));

      HSBucket<T> &bucket = lookup(dest, code);
      T *const value = (T *)&source.value;
      bool inserted = false;
      Eq eq;

      T *const result = bucket_insert(bucket, std::move(*value), inserted, eq);
      if (inserted) {
        assertx(result);
        assertx(node.entries > 0);
        --node.entries;
        ++dest.entries;
      }

      value->~T();
      source.present = false;
    }
  }

  source.next = rec::migrate_list<T, H, cap>(node, source.next, dest);
}

template <typename T, typename H, typename Eq, std::size_t cap>
static bool
rehash(HashSet<T, H, Eq> &self, HSNode<T, cap> &source) noexcept {
  HSNode<T, cap> *const other = split(self, source);
  assertx(verify(self.tree));

  if (other) {
    for (std::size_t i = 0; i < source.capacity; ++i) {
      migrate<T, H, Eq, cap>(source, source.buckets[i], *other);
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

// XXX combine these {
template <typename T, std::size_t cap, typename V, typename Eq>
static T *
node_insert(HSNode<T, cap> &node, const HashKey &c, V &&val, Eq eq) noexcept {
  assertx(in_range(node, c));

  HSBucket<T> &bucket = lookup(node, c);
  bool is_inserted = false;
  T *result = bucket_insert(bucket, std::forward<V>(val), is_inserted, eq);
  if (is_inserted) {
    assertx(result);
    ++node.entries;
  }

  return result;
}

template <typename T, std::size_t cap, typename V, typename Eq>
static T *
node_lookup_ins(HSNode<T, cap> &node, const HashKey &c, V &&val,
                Eq eq) noexcept {
  assertx(in_range(node, c));

  HSBucket<T> &bucket = lookup(node, c);
  bool is_inserted = false;
  T *result = bucket_lookup_ins(bucket, std::forward<V>(val), is_inserted, eq);
  if (is_inserted) {
    assertx(result);
    ++node.entries;
  }

  return result;
}

template <typename T, std::size_t cap, typename V, typename Eq>
static T *
node_upsert(HSNode<T, cap> &node, const HashKey &c, V &&val, Eq eq) noexcept {
  assertx(in_range(node, c));

  HSBucket<T> &bucket = lookup(node, c);
  bool is_inserted = false;
  T *result = bucket_upsert(bucket, std::forward<V>(val), is_inserted, eq);
  if (is_inserted) {
    assertx(result);
    ++node.entries;
  }

  return result;
}

template <typename T, std::size_t c, typename V, typename Factory, typename Eq>
static T *
node_lookup_compute(HSNode<T, c> &node, const HashKey &code, const V &needle,
                    Factory f, Eq eq) noexcept {
  assertx(in_range(node, code));

  HSBucket<T> &bucket = lookup(node, code);
  bool is_inserted = false;
  T *result = bucket_lookup_compute(bucket, needle, f, is_inserted, eq);
  if (is_inserted) {
    assertx(result);
    ++node.entries;
  }

  return result;
}
// }

template <typename T, typename H, typename Eq, typename V, typename Insert>
T *
do_insert(HashSet<T, H, Eq> &self, V &&val, Insert insert) noexcept {
  auto &tree = self.tree;
  H h;
  const HashKey code(h(val));

  HSNode<T> *node = find(tree, code);
  if (node == nullptr) {
    // should only get here on first invocation
    if (!is_empty(tree)) {
      dump(tree);
    }
    assertxs(is_empty(tree), sp::rec::length(self), code.hash);

    const std::size_t start = 0;
    // TODO bug when hash == length
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

    T *const result = insert(*node, code, std::forward<V>(val));
    if (result) {
      assertxs(code == h(*result), h(*result), code.hash);

      typename std::aligned_storage<sizeof(T), alignof(T)>::type wasd;
      std::memcpy(&wasd, result, sizeof(*result));

      // TODO resize factor
      if (node->entries >= node->capacity) {
        impl::rehash(self, *node);
      }
      // TODO why is never the data pointed to by $result affected by the
      // rehash?

      assertx(std::memcmp(&wasd, result, sizeof(*result)) == 0);
    }

    return result;
  }

  return nullptr;
}
} // namespace impl

template <typename T, typename H, typename Eq, typename V>
T *
insert(HashSet<T, H, Eq> &self, V &&val) noexcept {
  using namespace impl;

  auto f = [](HSNode<T> &node, const HashKey &code, V &&v) {
    Eq equality;
    return node_insert(node, code, std::forward<V>(v), equality);
  };

  return impl::do_insert(self, std::forward<V>(val), f);
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
upsert(HashSet<T, H, Eq> &self, V &&val) noexcept {
  using namespace impl;

  auto f = [](HSNode<T> &node, const HashKey &code, V &&v) {
    Eq equality;
    return node_upsert(node, code, std::forward<V>(v), equality);
  };

  return impl::do_insert(self, std::forward<V>(val), f);
}

//=====================================
namespace impl {
template <typename T, typename V, typename Eq>
static const T *
lookup(const HSBucket<T> &node, const V &needle, const Eq &eq) noexcept {
  const HSBucket<T> *current = &node;

Lit:
  if (current) {
    if (current->present) {
      const T *const value = (const T *)&current->value;
      if (eq(*value, needle)) {
        return value;
      }
    }

    current = current->next;
    goto Lit;
  }
  return nullptr;
}

template <typename T, std::size_t cap, typename V, typename Eq>
static const T *
lookup(const HSNode<T, cap> &node, const HashKey &c, const V &needle,
       const Eq &eq) noexcept {
  assertx(in_range(node, c));

  const HSBucket<T> &bucket = lookup(node, c);
  return lookup(bucket, needle, eq);
}

template <typename T, typename H, typename Eq, typename V>
const T *
lookup(const HashSet<T, H, Eq> &self, const HashKey &code,
       const V &needle) noexcept {
  const HSNode<T> *node = find(self.tree, code);
  if (node) {
    assertx(in_range(*node, code));

    Eq eq;
    return lookup(*node, code, needle, eq);
  }

  return nullptr;
}
} // namespace impl

template <typename T, typename Hash, typename Eq, typename V>
const T *
lookup(const HashSet<T, Hash, Eq> &self, const V &needle) noexcept {
  Hash h;
  const impl::HashKey code(h(needle));

  return impl::lookup(self, code, needle);
}

template <typename T, typename H, typename Eq, typename V>
T *
lookup(HashSet<T, H, Eq> &self, const V &needle) noexcept {
  const HashSet<T, H, Eq> &c_self = self;
  return (T *)lookup(c_self, needle);
}

//=====================================
template <typename T, typename H, typename Eq>
const T *
lookup_default(const HashSet<T, H, Eq> &self, const T &n,
               const T &def) noexcept {
  const T *result = lookup(self, n);
  if (!result) {
    result = &def;
  }

  return result;
}

template <typename T, typename H, typename Eq>
T *
lookup_default(HashSet<T, H, Eq> &self, const T &needle, T &def) noexcept {
  const auto &c_self = self;
  return (T *)lookup_default(c_self, needle, def);
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
T *
lookup_insert(HashSet<T, H, Eq> &self, V &&value) noexcept {
  using namespace impl;

  auto f = [](HSNode<T> &node, const HashKey &code, V &&v) {
    Eq equality;
    return node_lookup_ins(node, code, std::forward<V>(v), equality);
  };

  return impl::do_insert(self, std::forward<V>(value), f);
}

//=====================================
template <typename T, typename H, typename Eq, typename V, typename Compute>
T *
lookup_compute(HashSet<T, H, Eq> &self, const V &needle, Compute c) noexcept {
  using namespace impl;

  auto f = [c](HSNode<T> &node, const HashKey &code, const V &v) {
    Eq equality;
    return node_lookup_compute(node, code, v, c, equality);
  };

  return impl::do_insert(self, needle, f);
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
contains(const HashSet<T, H, Eq> &self, const V &needle) noexcept {
  return lookup(self, needle) != nullptr;
}

//=====================================
template <typename T, typename H, typename Eq, typename V>
bool
remove(HashSet<T, H, Eq> &self, const V &needle) noexcept {
  using namespace impl;

  H h;
  const HashKey code(h(needle));

  auto &tree = self.tree;
  HSNode<T> *const node = find(tree, code);
  if (node) {
    assertx(in_range(*node, code));

    HSBucket<T> &bucket = lookup(*node, code);
    Eq equality;

    return bucket_remove(bucket, needle, equality);
  }

  return false;
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
      const T *const value = (const T *)&it->value;
      f(*value);
    }
    it = it->next;
  }
}
} // namespace impl

template <typename T, typename H, typename Eq>
bool
verify(const sp::HashSet<T, H, Eq> &self) noexcept {
  using namespace impl;

  assertx(verify(self.tree));

  binary::rec::inorder(self.tree, [](const auto &node) {
    for_each(node, [&node](const auto &buckets) {
      for_each(buckets, [&node](const T &value) {
        H h;
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
template <typename T, typename H, typename Eq>
std::size_t
length(const HashSet<T, H, Eq> &self) noexcept {
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
