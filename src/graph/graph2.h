#ifndef SP_UTIL_GRAPH_GRAPH2_H
#define SP_UTIL_GRAPH_GRAPH2_H

#include <collection/Array.h>
#include <map/HashSet.h>
#include <queue/Queue.h>
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
    return target > o; // TODO??
  }

  bool
  operator>(const Edge<T, Weight> &o) const noexcept {
    assertx(target);
    assertx(o.target);
    return target > o.target; // TODO??
  }
};

template <typename T, typename Weight>
static bool
operator>(const Vertex<T> *f, const Edge<T, Weight> &s) noexcept {
  assertx(f);
  assertx(s.target);
  return f > s.target; // TODO??
}

//=====================================
template <typename T, typename Weight>
struct Vertex {
  T value;
  sp::UinStaticArray<Edge<T, Weight>, 256> edges;

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
template <typename T, typename W>
const Edge<T, W> *
get_edge(const Vertex<T, W> &, const Vertex<T, W> *) noexcept;

template <typename T, typename W>
Edge<T, W> *
get_edge(Vertex<T, W> &, const Vertex<T, W> *) noexcept;

//=====================================
template <typename T, typename W, typename F>
bool
deapth_first(Vertex<T, W> &, F) noexcept;

//=====================================
template <typename T, typename W, typename F>
bool
breadth_first(Vertex<T, W> &, F) noexcept;

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
  return get_edge(self, &needle) != nullptr;
}

//=====================================
template <typename T, typename W, typename WK>
bool
add_edge(Vertex<T, W> &self, WK &&weight, Vertex<T, W> *edge) noexcept {
  assertx(edge);
  return bin_insert_unique(self.edges,
                           Edge<T, W>(edge, std::forward<WK>(weight)));
}

//=====================================
template <typename T, typename W>
const Edge<T, W> *
get_edge(const Vertex<T, W> &self, const Vertex<T, W> *const needle) noexcept {
  assertx(needle);

  return bin_search(self.edges, needle);
}

template <typename T, typename W>
Edge<T, W> *
get_edge(Vertex<T, W> &self, const Vertex<T, W> *const needle) noexcept {
  const Vertex<T, W> &c_self = self;
  return (Edge<T, W> *)get_edge(c_self, needle);
}

//=====================================
namespace impl {
template <typename T, typename W>
struct vertex_hash {
  bool
  operator()(Vertex<T, W> *const &in) const noexcept {
    return reinterpret_cast<std::uintptr_t>(in);
  }
};

} // namespace impl

template <typename T, typename W, typename F>
bool
deapth_first(Vertex<T, W> &root, F f) noexcept {
  sp::HashSet<Vertex<T, W> *, impl::vertex_hash<T, W>> visited;

  sp::DynamicStack<Vertex<T, W> *> stack;
  if (!push(stack, &root)) {
    return false;
  }
  if (!insert(visited, &root)) {
    return false;
  }

  Vertex<T, W> *head = nullptr;
  while (pop(stack, head)) {
    assertx(head);
    f(*head);

    for (std::size_t i = 0; i < length(head->edges); ++i) {
      graph::Edge<T, W> &edge = head->edges[i];

      if (!lookup(visited, edge.target)) {
        if (!push(stack, edge.target)) {
          return false;
        }
        if (!insert(visited, edge.target)) {
          return false;
        }
      }

    } // for
  }   // while

  return true;
}

//=====================================
template <typename T, typename W, typename F>
bool
breadth_first(Vertex<T, W> &root, F f) noexcept {
  sp::HashSet<Vertex<T, W> *, impl::vertex_hash<T, W>> visited;

  sp::LinkedListQueue<Vertex<T, W> *> stack;
  if (!enqueue(stack, &root)) {
    return false;
  }
  if (!insert(visited, &root)) {
    return false;
  }

  Vertex<T, W> *head = nullptr;
  while (dequeue(stack, head)) {
    assertx(head);
    f(*head);

    for (std::size_t i = 0; i < length(head->edges); ++i) {
      graph::Edge<T, W> &edge = head->edges[i];

      if (!lookup(visited, edge.target)) {
        if (!enqueue(stack, edge.target)) {
          return false;
        }
        if (!insert(visited, edge.target)) {
          return false;
        }
      }

    } // for
  }   // while

  return true;
}

} // namespace graph
#endif
