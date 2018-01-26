#ifndef SP_UTIL_HEAP_BINARY_H
#define SP_UTIL_HEAP_BINARY_H

#include <util/comparator.h>

namespace heap {

// TODO emplace
// TODO do not require default constructable
// TODO dynamic binary
template <typename T, typename Comparator>
struct Binary {
  T *buffer;
  const std::size_t capacity;
  std::size_t length;

  Binary(T *, std::size_t) noexcept;
};

template <typename T>
using MaxBinary = Binary<T, sp::greater>;

template <typename T>
using MinBinary = Binary<T, sp::less>;

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &, V &&) noexcept;

// template<typename T,typename Comparator ,typename K>
// bool remove(Binary<T,Comparator>&,const K&) noexcept;

template <typename T, typename Comparator, typename K>
bool
take_head(Binary<T, Comparator> &, K &) noexcept;

template <typename T, typename Comparator>
T *
peek_head(Binary<T, Comparator> &) noexcept;

template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &, const K &) noexcept; // TODO

template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &, Binary<T, Comparator> &) noexcept; // TODO

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

template <typename T, typename Comp>
static void
swap(Binary<T, Comp> &heap, std::size_t idx1, std::size_t idx2) {
  std::swap(heap.buffer[idx1], heap.buffer[idx2]);
}

template <typename T, typename Comp>
static std::size_t
sift_up(Binary<T, Comp> &heap, std::size_t idx) noexcept {
Lit:
  if (idx == 0) {
    // we are root, we can not shift up further
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

template <typename T, typename Comp>
static std::size_t
extreme(Binary<T, Comp> &heap, std::size_t first_idx,
        std::size_t second_idx) noexcept {
  constexpr Comp c;
  /*
   * either: greather than or less than
   */
  if (c(heap.buffer[first_idx], heap.buffer[second_idx])) {
    return first_idx;
  }
  return second_idx;
}

template <typename T, typename Comp>
static void
shift_down(Binary<T, Comp> &heap, std::size_t idx) noexcept {
Lit:
  std::size_t max_idx = heap.length;

  const std::size_t left_idx = left_child(idx);
  if (left_idx < heap.length) {
    max_idx = left_idx;

    const std::size_t right_idx = right_child(idx);
    if (right_idx < heap.length) {
      max_idx = extreme(heap, right_idx, left_idx);
    }
  }

  if (max_idx < heap.length) {

    constexpr Comp c;
    if (c(heap.buffer[max_idx], heap.buffer[idx])) {
      swap(heap, idx, max_idx);
      idx = max_idx;
      goto Lit;
    }
  }
}
} // namespace heap
} // namespace impl

template <typename T, typename Comparator>
Binary<T, Comparator>::Binary(T *b, std::size_t c) noexcept
    : buffer{b}
    , capacity{c}
    , length(0) {
}

template <typename T, typename Comparator, typename V>
T *
insert(Binary<T, Comparator> &heap, V &&val) noexcept {
  using namespace impl::heap;
  if (heap.length < heap.capacity) {
    std::size_t idx = heap.length++;

    {
      T *const dest = heap.buffer + idx;
      dest->~T();
      new (dest) T(std::forward<V>(val));
      // heap.buffer[idx] = std::forward<V>(val);
    }

    idx = sift_up(heap, idx);
    return heap.buffer + idx;
  }

  return nullptr;
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
T *
peek_head(Binary<T, Comparator> &heap) noexcept {
  if (heap.length > 0) {
    return &heap.buffer[0];
  }

  return nullptr;
}

template <typename T, typename Comparator, typename K>
T *
find(Binary<T, Comparator> &, const K &) noexcept {
  return nullptr; // TODO
}

template <typename T, typename Comparator>
void
swap(Binary<T, Comparator> &first, Binary<T, Comparator> &second) noexcept {
  std::swap(first.length, second.length);
  std::swap(first.buffer, second.buffer);
  std::swap(first.capacity, second.capacity);
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
