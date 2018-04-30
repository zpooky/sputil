#ifndef SP_UTIL_HEAP_BINARY_H
#define SP_UTIL_HEAP_BINARY_H

#include <stack/Stack.h>
#include <util/comparator.h>
#include <util/assert.h>

namespace heap {

/*
 * TODO emplace
 * TODO do not require default constructable
 * TODO dynamic binary
 *
 */
template <typename T, typename Comparator>
struct Binary {
  T *buffer;
  std::size_t capacity;
  std::size_t length;

  Binary(T *, std::size_t) noexcept;
};

template <typename T, std::size_t N, typename Comparator>
struct StaticBinary : public Binary<T, Comparator> {
  T raw[N];

  StaticBinary() noexcept;
};

template <typename T>
using MaxBinary = Binary<T, sp::greater>;

template <typename T, std::size_t N>
using StaticMaxBinary = StaticBinary<T, N, sp::greater>;

template <typename T>
using MinBinary = Binary<T, sp::less>;

template <typename T, std::size_t N>
using StaticMinBinary = StaticBinary<T, N, sp::less>;

template <typename T, typename Comparator>
bool
is_empty(const Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator>
std::size_t
length(const Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator>
std::size_t
capacity(const Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator, typename V>
T *
insert_lazy(Binary<T, Comparator> &, V &&) noexcept;

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &, V &&) noexcept;

// template <typename T, std::size_t N, typename Comparator, typename V>
// T *
// insert_eager(StaticBinary<T, N, Comparator> &, V &&) noexcept;

// TODO verify last only iterates over leaf nodes
template <typename T, typename Comparator>
const T *
last(const Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator>
T *
last(Binary<T, Comparator> &) noexcept;

// template<typename T,typename Comparator ,typename K>
// bool remove(Binary<T,Comparator>&,const K&) noexcept;

template <typename T, typename Comparator, typename K>
bool
take_head(Binary<T, Comparator> &, K &) noexcept;

template <typename T, typename Comparator>
bool
drop_head(Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator>
T *
peek_head(Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &, const K &) noexcept; // TODO

template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &, Binary<T, Comparator> &) noexcept;

// create a heap out of given array of elements
template <typename T, typename Comparator>
Binary<T, Comparator>
heapify(T *, std::size_t) noexcept; // TODO

// TODO define
// // joining two heaps to form a valid new heap containing all the elements
// shared by both parties template<typename T,typename Comparator > bool
// union(Binary<T,Comarator>&dest,const Binary<T,Comparator>&source)
// noexcept;//TODO
//
// template<typename T,typename Comparator >
// bool union(Binary<T,Comarator>&dest,const Binary<T,Comparator>&&source)
// noexcept;//TODO

// joining two heaps to form a valid new heap containing all the elements of
// both
template <typename T, typename Comparator>
bool
concat(Binary<T, Comparator> &dest,
       const Binary<T, Comparator> &source) noexcept; // TODO

template <typename T, typename Comparator>
bool
concat(Binary<T, Comparator> &dest,
       const Binary<T, Comparator> &&source) noexcept; // TODO

template <typename Comparator>
void
dump(Binary<int, Comparator> &, std::size_t idx = 0, std::string prefix = "",
     bool isTail = true, const char *ctx = "") noexcept;

/*
 * ==========================================================================
 */
namespace impl {
namespace heap {

inline std::size_t
parent(std::size_t idx) noexcept {
  return (idx - 1) / 2;
}

inline std::size_t
left_child(std::size_t idx) noexcept {
  return (2 * idx) + 1;
}

inline std::size_t
right_child(std::size_t idx) noexcept {
  return (2 * idx) + 2;
}

template <typename T, typename Comp>
static void
swap(Binary<T, Comp> &heap, std::size_t idx1, std::size_t idx2) {
  using std::swap;
  swap(heap.buffer[idx1], heap.buffer[idx2]);
}

template <typename T, typename Comp>
static std::size_t
shift_up(Binary<T, Comp> &heap, std::size_t idx) noexcept {
Lit:
  if (idx == 0) {
    /* we are root, we can not shift up further */
    return idx;
  }

  constexpr Comp c;
  std::size_t parent_idx = parent(idx);
  if (c(heap.buffer[idx], heap.buffer[parent_idx])) {
    swap(heap, idx, parent_idx);
    idx = parent_idx;
    goto Lit;
  }
  return idx;
}

template <typename T, typename Comp, typename V>
static T *
insert_at(Binary<T, Comp> &heap, std::size_t idx, V &&val) noexcept {
  // assertx(idx < capacity(heap));

  T *const dest = heap.buffer + idx;
  dest->~T();
  new (dest) T(std::forward<V>(val));

  idx = shift_up(heap, idx);
  return heap.buffer + idx;
}

template <typename T, typename Comp>
std::size_t
extreme(Binary<T, Comp> &heap, std::size_t first, std::size_t second) noexcept {
  assertx(first < heap.length);
  assertx(second < heap.length);
  constexpr Comp c;
  /* either: greater than or less than */
  if (c(heap.buffer[first], heap.buffer[second])) {
    return first;
  }

  return second;
}

template <typename T, typename Comp>
static void
shift_down(Binary<T, Comp> &heap, std::size_t idx) noexcept {
Lit:
  std::size_t extreme_idx = heap.length;

  const std::size_t left_idx = left_child(idx);
  if (left_idx < heap.length) {
    extreme_idx = left_idx;

    const std::size_t right_idx = right_child(idx);
    if (right_idx < heap.length) {
      extreme_idx = extreme(heap, right_idx, left_idx);
    }
  }

  if (extreme_idx < heap.length) {

    constexpr Comp c;
    if (c(heap.buffer[extreme_idx], heap.buffer[idx])) {
      swap(heap, idx, extreme_idx);
      idx = extreme_idx;
      goto Lit;
    }
  }
}

template <typename T, typename Comparator>
std::size_t
index_of(const Binary<T, Comparator> &heap, T *ptr) noexcept {
  assertx(ptr);
  if (heap.buffer) {
    const T *start = heap.buffer;
    auto s = reinterpret_cast<std::uintptr_t>(start);
    auto e = reinterpret_cast<std::uintptr_t>(ptr);
    if (e >= s) {
      auto index = e - s;
      index /= sizeof(*ptr);
      if (index < heap.length) {
        return index;
      }
    }
  }
  return heap.capacity;
}

} // namespace heap
} // namespace impl

template <typename T, typename Comparator>
Binary<T, Comparator>::Binary(T *b, std::size_t c) noexcept
    : buffer{b}
    , capacity{c}
    , length(0) {
}

template <typename T, std::size_t N, typename Comparator>
StaticBinary<T, N, Comparator>::StaticBinary() noexcept
    : Binary<T, Comparator>(raw, N)
    , raw() {
}

template <typename T, typename Comparator>
bool
is_empty(const Binary<T, Comparator> &heap) noexcept {
  return heap.length == 0;
}

template <typename T, typename Comparator>
std::size_t
length(const Binary<T, Comparator> &heap) noexcept {
  return heap.length;
}

template <typename T, typename Comparator>
std::size_t
capacity(const Binary<T, Comparator> &heap) noexcept {
  return heap.capacity;
}

template <typename T, typename Comparator, typename V>
T *
insert_lazy(Binary<T, Comparator> &heap, V &&val) noexcept {
  using namespace impl::heap;
  if (heap.length < heap.capacity) {
    std::size_t idx = heap.length++;
    return insert_at(heap, idx, std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &heap, V &&val) noexcept {
  return insert_lazy(heap, std::forward<V>(val));
}

template <typename T, std::size_t N, typename Comparator, typename V>
T *
insert_eager(StaticBinary<T, N, Comparator> &heap, V &&val) noexcept {
  using namespace impl::heap;

  if (heap.length == heap.capacity && heap.capacity > 0) {
    T *l = last(heap);
    assertx(l);
    std::size_t idx = index_of(heap, l);
    assertx(idx != heap.capacity);

    constexpr Comparator cmp;
    if (cmp(val, *l)) {

      using std::swap;
      T copy(std::forward<V>(val));
      swap(heap.buffer[idx], copy);
      idx = shift_up(heap, idx);
      return heap.buffer + idx;
    }

    return nullptr;
  }

  return insert_lazy(heap, std::forward<V>(val));
}

template <typename T, typename Comparator>
const T *
last(const Binary<T, Comparator> &heap) noexcept {
  /* Itterate over leafs */
  const T *result = nullptr;
  for (std::size_t i = ((heap.length + 1) / 2) - 1; i < heap.length; ++i) {

    constexpr Comparator cmp;
    // if (c(heap.buffer[extreme_idx], heap.buffer[idx])) {
    if (result == nullptr || cmp(*result, heap.buffer[i])) {
      result = heap.buffer + i;
    }
  }
  return result;
}

template <typename T, typename Comparator>
T *
last(Binary<T, Comparator> &heap) noexcept {
  const auto &c_heap = heap;
  return (T *)last(c_heap);
}

template <typename T, typename Comparator, typename K>
bool
take_head(Binary<T, Comparator> &heap, K &swp) noexcept {
  using namespace impl::heap;
  if (heap.length > 0) {
    std::size_t idx = 0;
    std::size_t last_idx = heap.length - 1;

    std::swap(heap.buffer[idx], swp);
    std::swap(heap.buffer[idx], heap.buffer[last_idx]);
    heap.length--;

    shift_down(heap, idx);

    return true;
  }
  return false;
}

template <typename T, typename Comparator>
bool
drop_head(Binary<T, Comparator> &heap) noexcept {
  T out;
  return take_head(heap, out);
}

template <typename T, typename Comparator>
T *
peek_head(Binary<T, Comparator> &heap) noexcept {
  if (heap.length > 0) {
    return &heap.buffer[0];
  }

  return nullptr;
}

template <typename T, typename Comparator, typename K>
T *
find_heap(Binary<T, Comparator> &heap, const K &needle) noexcept {
  using namespace impl::heap;

  // std::size_t raw[1024] = {0};
  auto raw = new std::size_t[1024];
  sp::Stack<std::size_t> stack{raw, 1024};
  push(stack, 0);
  std::size_t index;
  while (pop(stack, index)) {

  Lit:
    if (index < heap.length) {
      constexpr Comparator cmp;
      const bool greater = cmp(heap.buffer[index], needle);
      const bool lesser = cmp(needle, heap.buffer[index]);

      if (!greater && !lesser) { //==
        delete raw;
        return &heap.buffer[index];
      } else if (greater) {
        std::size_t left = left_child(index);
        if (left < heap.length) {
          if (!push(stack, left)) {
            delete raw;
            assertx(false);
            return nullptr;
          }
        }

        // We directly handle right child, so we do not need to keep track of
        // it on the stack.
        std::size_t right = right_child(index);
        index = right;
        goto Lit;
      }
    }
  }

  delete raw;
  return nullptr; // TODO
}

template <typename T, typename Comparator, typename K>
T *
find_stack(Binary<T, Comparator> &heap, const K &needle,
           std::size_t index) noexcept {
  using namespace impl::heap;

  if (index < heap.length) {
    constexpr Comparator cmp;
    const bool greater = cmp(heap.buffer[index], needle);
    const bool lesser = cmp(needle, heap.buffer[index]);

    if (!greater && !lesser) { //==
      return &heap.buffer[index];
    } else if (greater) {
      T *l = find_stack(heap, needle, left_child(index));
      if (l != nullptr) {
        return l;
      }
      T *r = find_stack(heap, needle, right_child(index));
      if (r != nullptr) {
        return r;
      }
    }
  }

  return nullptr; // TODO
}

template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &heap, const K &needle) noexcept {
  // return find_stack(heap,needle,0);
  return find_heap(heap, needle);
}

template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &first, Binary<T, Comparator> &second) noexcept {
  using sp::swap;
  swap(first.buffer, second.buffer);
  swap(first.length, second.length);
  swap(first.capacity, second.capacity);
}

template <typename Comparator>
void
dump(Binary<int, Comparator> &heap, std::size_t idx, std::string prefix,
     bool isTail, const char *ctx) noexcept {
  using namespace impl::heap;
  if (idx < heap.length) {
    char name[256] = {0};
    sprintf(name, "%s%d", ctx, heap.buffer[idx]);

    printf("%s%s%s\n", prefix.c_str(), (isTail ? "└── " : "├── "), name);

    const char *ls = (isTail ? "    " : "│   ");

    bool has_left = left_child(idx) < heap.length;
    bool has_right = right_child(idx) < heap.length;

    if (has_left && has_right) {
      dump(heap, right_child(idx), prefix + ls, false);
      dump(heap, left_child(idx), prefix + ls, true);
    } else {
      if (has_left) {
        dump(heap, left_child(idx), prefix + ls, true);
      } else if (has_right) {
        dump(heap, right_child(idx), prefix + ls, true);
      }
    }
  }
} // heap::dump()

} // namespace heap

#endif
