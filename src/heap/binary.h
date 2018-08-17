#ifndef SP_UTIL_HEAP_BINARY_H
#define SP_UTIL_HEAP_BINARY_H

#include <stack/HeapStack.h>
#include <util/assert.h>
#include <util/comparator.h>

namespace heap {

/*
 * TODO emplace
 * TODO do not require default constructable
 * TODO dynamic binary
 *
 */
//=====================================
template <typename T, typename Comparator>
struct Binary {
  T *buffer;
  std::size_t capacity;
  std::size_t length;

  Binary(T *, std::size_t capacity) noexcept;
};

//=====================================
template <typename T, std::size_t N, typename Comparator>
struct StaticBinary : public Binary<T, Comparator> {
  T raw[N];

  StaticBinary() noexcept;
};

//=====================================
template <typename T>
using MaxBinary = Binary<T, sp::greater>;

template <typename T, std::size_t N>
using StaticMaxBinary = StaticBinary<T, N, sp::greater>;

template <typename T>
using MinBinary = Binary<T, sp::less>;

template <typename T, std::size_t N>
using StaticMinBinary = StaticBinary<T, N, sp::less>;

//=====================================
template <typename T, typename Comparator>
bool
is_empty(const Binary<T, Comparator> &) noexcept;

//=====================================
template <typename T, typename Comparator>
bool
is_full(const Binary<T, Comparator> &) noexcept;

//=====================================
template <typename T, typename Comparator>
std::size_t
length(const Binary<T, Comparator> &) noexcept;

//=====================================
template <typename T, typename Comparator>
std::size_t
capacity(const Binary<T, Comparator> &) noexcept;

//=====================================
/*
 * Does not insert value if heap is full
 */
template <typename T, typename Comparator, typename V>
T *
insert_lazy(Binary<T, Comparator> &, V &&) noexcept;

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &, V &&) noexcept;

/*
 * If heap is full it drops a lower priority value present in the heap be able
 * to fit the new value.
 */
template <typename T, typename Comparator, typename V>
T *
insert_eager(Binary<T, Comparator> &, V &&) noexcept;

//=====================================
// TODO verify last only iterates over leaf nodes
template <typename T, typename Comparator>
const T *
last(const Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator>
T *
last(Binary<T, Comparator> &) noexcept;

//=====================================
// template<typename T,typename Comparator ,typename K>
// bool remove(Binary<T,Comparator>&,const K&) noexcept;

//=====================================
template <typename T, typename Comparator, typename K>
bool
take_head(Binary<T, Comparator> &, K &) noexcept;

//=====================================
template <typename T, typename Comparator>
bool
drop_head(Binary<T, Comparator> &) noexcept;

//=====================================
template <typename T, typename Comparator>
T *
peek_head(Binary<T, Comparator> &) noexcept;

//=====================================
template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &, const K &) noexcept; // TODO

//=====================================
template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &, Binary<T, Comparator> &) noexcept;
//TODO this should not work a static heap

//=====================================
// create a heap out of given array of N unsorted elements
template <typename T, typename Comparator>
Binary<T, Comparator>
heapify(T *, std::size_t) noexcept;

//=====================================
// TODO define
// // joining two heaps to form a valid new heap containing all the elements
// shared by both parties template<typename T,typename Comparator > bool
// union(Binary<T,Comarator>&dest,const Binary<T,Comparator>&source)
// noexcept;//TODO
//
// template<typename T,typename Comparator >
// bool union(Binary<T,Comarator>&dest,const Binary<T,Comparator>&&source)
// noexcept;//TODO

//=====================================
// joining two heaps to form a valid new heap containing all the elements of
// both
template <typename T, typename Comparator>
bool
concat(Binary<T, Comparator> &dest,
       const Binary<T, Comparator> &source) noexcept; // TODO

//=====================================
template <typename Comparator>
void
dump(Binary<int, Comparator> &, std::size_t idx = 0, std::string prefix = "",
     bool isTail = true, const char *ctx = "") noexcept;

//=====================================
//====Implementation===================
//=====================================
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
static std::size_t
shift_up(Binary<T, Comp> &self, std::size_t idx) noexcept {
Lit:
  if (idx == 0) {
    /* we are root, we can not shift up further */
    return idx;
  }

  constexpr Comp c;
  std::size_t parent_idx = parent(idx);
  if (c(self.buffer[idx], /*>*/ self.buffer[parent_idx])) {
    using std::swap;
    swap(self.buffer[idx], self.buffer[parent_idx]);
    idx = parent_idx;
    goto Lit;
  }

  return idx;
}

template <typename T, typename Comp, typename V>
static T *
insert_at(Binary<T, Comp> &self, std::size_t idx, V &&val) noexcept {
  assertxs(idx < capacity(self), idx, capacity(self));

  T *const dest = self.buffer + idx;
  dest->~T();
  new (dest) T(std::forward<V>(val));

  idx = shift_up(self, idx);
  return self.buffer + idx;
}

template <typename T, typename Comp>
std::size_t
extreme(Binary<T, Comp> &self, std::size_t first, std::size_t second) noexcept {
  assertxs(first < self.length, first, self.length);
  assertxs(second < self.length, first, self.length);

  constexpr Comp c;
  if (c(self.buffer[first], /*>*/ self.buffer[second])) {
    return first;
  }

  return second;
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

//=====================================
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

//=====================================
template <typename T, typename Comparator>
bool
is_empty(const Binary<T, Comparator> &self) noexcept {
  return self.length == 0;
}

//=====================================
template <typename T, typename Comparator>
bool
is_full(const Binary<T, Comparator> &self) noexcept {
  return self.length == self.capacity;
}

//=====================================
template <typename T, typename Comparator>
std::size_t
length(const Binary<T, Comparator> &self) noexcept {
  return self.length;
}

//=====================================
template <typename T, typename Comparator>
std::size_t
capacity(const Binary<T, Comparator> &self) noexcept {
  return self.capacity;
}

//=====================================
template <typename T, typename Comparator, typename V>
T *
insert_lazy(Binary<T, Comparator> &self, V &&val) noexcept {
  using namespace impl::heap;
  if (!is_full(self)) {

    std::size_t idx = self.length++;
    return insert_at(self, idx, std::forward<V>(val));
  }

  return nullptr;
}

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &self, V &&val) noexcept {
  return insert_lazy(self, std::forward<V>(val));
}

template <typename T, typename Comparator, typename V>
T *
insert_eager(Binary<T, Comparator> &self, V &&val) noexcept {
  using namespace impl::heap;

  if (is_full(self) && self.capacity > 0) {
    T *l = last(self);
    assertx(l);
    std::size_t idx = index_of(self, l);
    assertx(idx != self.capacity);

    constexpr Comparator cmp;
    if (cmp(val, *l)) {

      using std::swap;
      T copy(std::forward<V>(val));
      swap(self.buffer[idx], copy);
      idx = shift_up(self, idx);
      return self.buffer + idx;
    }

    return nullptr;
  }

  return insert_lazy(self, std::forward<V>(val));
}

//=====================================
template <typename T, typename Comparator>
const T *
last(const Binary<T, Comparator> &self) noexcept {
  /* Itterate over leafs */
  const T *result = nullptr;
  for (std::size_t i = ((self.length + 1) / 2) - 1; i < self.length; ++i) {

    constexpr Comparator cmp;
    // if (c(self.buffer[extreme_idx], self.buffer[idx])) {
    if (result == nullptr || cmp(*result, self.buffer[i])) {
      result = self.buffer + i;
    }
  }
  return result;
}

template <typename T, typename Comparator>
T *
last(Binary<T, Comparator> &self) noexcept {
  const auto &c_heap = self;
  return (T *)last(c_heap);
}

//=====================================
namespace impl {
namespace heap {
template <typename T, typename Comp>
static void
shift_down(Binary<T, Comp> &self, std::size_t idx) noexcept {
Lit:
  std::size_t extreme_idx = self.length;

  const std::size_t left_idx = left_child(idx);
  if (left_idx < self.length) {
    extreme_idx = left_idx;

    const std::size_t right_idx = right_child(idx);
    if (right_idx < self.length) {
      extreme_idx = extreme(self, right_idx, left_idx);
    }
  }

  if (extreme_idx < self.length) {

    constexpr Comp c;
    if (c(self.buffer[extreme_idx], self.buffer[idx])) {
      using std::swap;
      swap(self.buffer[idx], self.buffer[extreme_idx]);
      idx = extreme_idx;
      goto Lit;
    }
  }
}
}
}

template <typename T, typename Comparator, typename K>
bool
take_head(Binary<T, Comparator> &self, K &out) noexcept {
  if (!is_empty(self)) {
    const std::size_t head = 0;
    const std::size_t last = self.length - 1;

    using std::swap;
    swap(self.buffer[head], out);
    swap(self.buffer[head], self.buffer[last]);
    self.length--;

    impl::heap::shift_down(self, head);

    return true;
  }

  return false;
}

//=====================================
template <typename T, typename Comparator>
bool
drop_head(Binary<T, Comparator> &self) noexcept {
  T out;
  return take_head(self, out);
}

//=====================================
template <typename T, typename Comparator>
T *
peek_head(Binary<T, Comparator> &self) noexcept {
  if (!is_empty(self)) {
    return &self.buffer[0];
  }

  return nullptr;
}

//=====================================
namespace impl {
template <typename T, typename Comparator, typename K>
T *
find_heap(Binary<T, Comparator> &self, const K &needle) noexcept {
  using namespace impl::heap;

  sp::HeapStack<std::size_t> stack;
  push(stack, 0);
  std::size_t index;
  while (pop(stack, index)) {

  Lit:
    if (index < self.length) {
      constexpr Comparator cmp;
      const bool greater = cmp(self.buffer[index], needle);
      const bool lesser = cmp(needle, self.buffer[index]);

      if (!greater && !lesser) { //==
        return self.buffer + index;
      } else if (greater) {
        std::size_t left = left_child(index);
        if (left < self.length) {
          if (!push(stack, left)) {
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

  return nullptr; // TODO
}

template <typename T, typename Comparator, typename K>
T *
find_stack(Binary<T, Comparator> &self, const K &needle,
           std::size_t index) noexcept {
  using namespace impl::heap;

  /* DFS */
  if (index < self.length) {
    constexpr Comparator cmp;
    const bool greater = cmp(self.buffer[index], needle);
    const bool lesser = cmp(needle, self.buffer[index]);

    if /*==*/(!greater && !lesser) {
      return self.buffer + index;
    } else if (greater) {
      T *l = find_stack(self, needle, left_child(index));
      if (l != nullptr) {
        return l;
      }
      T *r = find_stack(self, needle, right_child(index));
      if (r != nullptr) {
        return r;
      }
    }
  }

  return nullptr; // TODO
}
}

template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &self, const K &needle) noexcept {
  // return impl::find_stack(self,needle,0);
  return impl::find_heap(self, needle);
}

//=====================================
template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &first, Binary<T, Comparator> &second) noexcept {
  using std::swap;
  swap(first.buffer, second.buffer);
  swap(first.length, second.length);
  swap(first.capacity, second.capacity);
}

//=====================================
template <typename T, typename Comparator>
Binary<T, Comparator>
heapify(T *const raw, std::size_t length) noexcept {
  using namespace impl::heap;

  Binary<T, Comparator> heap(raw, /*cap*/length);
  for (std::size_t i = 0; i < length; ++i) {
    std::size_t idx = heap.length++;
    shift_up(heap, idx);
  }

  return heap;
}

//=====================================
template <typename Comparator>
void
dump(Binary<int, Comparator> &self, std::size_t idx, std::string prefix,
     bool isTail, const char *ctx) noexcept {
  using namespace impl::heap;
  if (idx < self.length) {
    char name[256] = {0};
    sprintf(name, "%s%d", ctx, self.buffer[idx]);

    printf("%s%s%s\n", prefix.c_str(), (isTail ? "└── " : "├── "), name);

    const char *ls = (isTail ? "    " : "│   ");

    bool has_left = left_child(idx) < self.length;
    bool has_right = right_child(idx) < self.length;

    if (has_left && has_right) {
      dump(self, right_child(idx), prefix + ls, false);
      dump(self, left_child(idx), prefix + ls, true);
    } else {
      if (has_left) {
        dump(self, left_child(idx), prefix + ls, true);
      } else if (has_right) {
        dump(self, right_child(idx), prefix + ls, true);
      }
    }
  }
} // heap::dump()

//=====================================
} // namespace heap

#endif
