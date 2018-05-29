#ifndef SP_UTIL_GRAPH_GRAPH2_H
#define SP_UTIL_GRAPH_GRAPH2_H

#include <collection/Array.h>
#include <utility>

namespace graph {
//=====================================
template <typename T, typename Weight = int>
struct Vertex;

//=====================================
template <typename T, typename Weight>
struct Edge {
  Vertex<T> *target;
  Weight weight;

  template <typename WK>
  Edge(Vertex<T> *t, WK &&w)
      : target{t}
      , weight{std::forward<WK>(w)} {
  }

  bool
  operator>(const Vertex<T> *o) const noexcept {
    assertx(o);
    assertx(target);
    return target > o;
  }

  bool
  operator>(const Edge<T, Weight> &o) const noexcept {
    assertx(target);
    assertx(o.target);
    return target > o.target;
  }
};

template <typename T, typename Weight>
static bool
operator>(const Vertex<T> *f, const Edge<T, Weight> &s) noexcept {
  assertx(f);
  assertx(s.target);
  return f > s.target;
}

//=====================================
template <typename T, typename Weight>
struct Vertex {
  T value;
  sp::UinStaticArray<Edge<T, Weight>, 128> edges;

  template <typename A>
  Vertex(A &&) noexcept;

  ~Vertex() noexcept;
};

//=====================================
template <typename T, typename W>
bool
is_adjacent(const Vertex<T, W> &, const Vertex<T, W> &) noexcept;

//=====================================
template <typename T, typename W, typename WK>
bool
add_edge(Vertex<T, W> &, WK &&, Vertex<T, W> *) noexcept;

//=====================================
//====Implementation===================
//=====================================

template <typename T, typename Weight>
template <typename A>
Vertex<T, Weight>::Vertex(A &&a) noexcept
    : value{std::forward<A>(a)}
    , edges{} {
}

template <typename T, typename Weight>
Vertex<T, Weight>::~Vertex() noexcept {
}

//=====================================
template <typename T, typename W>
bool
is_adjacent(const Vertex<T, W> &self, const Vertex<T, W> &needle) noexcept {
  return bin_search(self.edges, &needle);
}

//=====================================
template <typename T, typename W, typename WK>
bool
add_edge(Vertex<T, W> &self, WK &&weight, Vertex<T, W> *edge) noexcept {
  assertx(edge);
  return bin_insert_unique(self.edges,
                           Edge<T, W>(edge, std::forward<WK>(weight)));
}

} // namespace graph

#endif
