#ifndef SP_UTIL_GRAPH_GRAPH_H
#define SP_UTIL_GRAPH_GRAPH_H

#include <utility>
#include <cstddef>
#include <cstdint>

namespace graph {
//=====================================
template <typename Vertex>
struct Wrapper {
  using edge_type = Vertex;
  Vertex *ptr;
  bool owner;

  explicit Wrapper(const edge_type *) noexcept;
  Wrapper(Wrapper<Vertex> &&) noexcept;

  Wrapper(const Wrapper<Vertex> &) = delete;
  Wrapper<Vertex> &
  operator=(const Wrapper<Vertex> &) = delete;

  Wrapper<Vertex> &
  operator=(Wrapper<Vertex> &&o) noexcept;

  bool
  operator>(const Wrapper<Vertex> &) const noexcept;

  bool
  operator>(const Wrapper<Vertex> *) const noexcept;

  bool
  operator>(const edge_type *) const noexcept;

  bool
  operator>(const edge_type &) const noexcept;

  bool
  operator==(const edge_type *) const noexcept;

  ~Wrapper() noexcept;
};

//=====================================
//====Implementation===================
//=====================================
template <typename Vertex>
Wrapper<Vertex>::Wrapper(const edge_type *a) noexcept
    : ptr((edge_type *)a)
    , owner(false) {
}

template <typename Vertex>
Wrapper<Vertex>::Wrapper(Wrapper<Vertex> &&o) noexcept
    : ptr(o.ptr)
    , owner(o.owner) {
  o.owner = false;
  o.ptr = nullptr;
}

template <typename Vertex>
Wrapper<Vertex> &
Wrapper<Vertex>::operator=(Wrapper<Vertex> &&o) noexcept {
  using std::swap;
  swap(ptr, o.ptr);
  swap(owner, o.owner);
  return *this;
}

template <typename Vertex>
bool
Wrapper<Vertex>::operator>(const Wrapper<Vertex> &o) const noexcept {
  return operator>(o.ptr);
}

template <typename Vertex>
bool
Wrapper<Vertex>::operator>(const Wrapper<Vertex> *o) const noexcept {
  assertx(o);
  return operator>(o->ptr);
}

template <typename Vertex>
bool
Wrapper<Vertex>::operator>(const edge_type *o) const noexcept {
  assertx(o);
  uintptr_t first = reinterpret_cast<uintptr_t>(ptr);
  uintptr_t second = reinterpret_cast<uintptr_t>(o);
  return first > second;
}

template <typename Vertex>
bool
Wrapper<Vertex>::operator>(const edge_type &o) const noexcept {
  return operator>(o.ptr);
}

template <typename Vertex>
bool
Wrapper<Vertex>::operator==(const edge_type *o) const noexcept {
  assertx(o);
  return ptr == o;
}

template <typename Vertex>
Wrapper<Vertex>::~Wrapper() noexcept {
  if (owner) {
    assertx(ptr);
    if (ptr) {
      delete ptr;
    }
    owner = false;
  }
  ptr = nullptr;
}

} // namespace graph

#endif
