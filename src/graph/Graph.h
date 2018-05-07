#ifndef SP_UTIL_GRAPH_GRAPH_H
#define SP_UTIL_GRAPH_GRAPH_H

#include <collection/Array.h>
#include <utility>

/*
 * http://web.cecs.pdx.edu/~sheard/course/Cs163/Doc/Graphs.html
 *
 */
namespace graph {
// TODO make implicit converse to Unidrect work

template <typename T, std::size_t N>
struct Undirected;

template <typename T, std::size_t N>
struct Wrapper {
  using edge_type = Undirected<T, N>;
  edge_type *ptr;
  bool owner;

  Wrapper(const edge_type *a) noexcept
      : ptr((edge_type *)a)
      , owner(false) {
  }

  Wrapper(Wrapper<T, N> &&o) noexcept
      : ptr(o.ptr)
      , owner(o.owner) {
    o.owner = false;
    o.ptr = nullptr;
  }

  Wrapper(const Wrapper<T, N> &) = delete;
  Wrapper<T, N> &
  operator=(const Wrapper<T, N> &) = delete;

  Wrapper<T, N> &
  operator=(Wrapper<T, N> &&o) noexcept {
    using std::swap;
    swap(ptr, o.ptr);
    swap(owner, o.owner);
    return *this;
  }

  bool
  operator>(const Wrapper<T, N> &o) const noexcept {
    return operator>(o.ptr);
  }

  bool
  operator>(const Wrapper<T, N> *o) const noexcept {
    return operator>(o->ptr);
  }

  bool
  operator>(const edge_type *o) const noexcept {
    return ptr > o;
  }

  bool
  operator>(const edge_type &o) const noexcept {
    return operator>(o.ptr);
  }

  ~Wrapper() noexcept {
    if (owner) {
      assertx(ptr);
      if (ptr) {
        delete ptr;
      }
      owner = false;
    }
    ptr = nullptr;
  }
};
/*
 * <->
 */
template <typename T, std::size_t N = 5>
struct Undirected {
  static_assert(N >= 1, "");
  // using edge_type = Undirected<T, N> *;
  using edge_type = Wrapper<T, N>;
  using edges_type = sp::UinStaticArray<edge_type, N>;
  static constexpr std::size_t capacity = N;

  T value;
  edges_type edges;
  /**/
  template <typename A>
  Undirected(A &&) noexcept;

  ~Undirected() noexcept;
};

/*
 * ->
 */
struct Directed {
  /**/
};

/*
 *  N
 * <->
 */
struct Weighted {
  /**/
};
} // namespace graph

namespace dijkstra {
// void
// shortest_path(graph::Weighted *from, graph::Weighted *to) noexcept;
// // invert weight
// void
// longest_path(graph::Weighted *from, graph::Weighted *to) noexcept;
} // namespace dijkstra

namespace graph {

//=====================================
template <typename T, std::size_t N, typename F>
void
bfs(Undirected<T, N> &, F) noexcept;

template <typename T, std::size_t N, typename F>
void
bfs(const Undirected<T, N> &, F) noexcept;

//=====================================
template <typename T, std::size_t N, typename F>
void
dfs(Undirected<T, N> &, F) noexcept;

template <typename T, std::size_t N, typename F>
void
dfs(const Undirected<T, N> &, F) noexcept;

//=====================================
// TODO emplace_vertex()
template <typename T, std::size_t N, typename A>
Undirected<T, N> *
add_vertex(Undirected<T, N> &, A &&) noexcept;

template <typename T, std::size_t N>
bool
add_edge(Undirected<T, N> &, Undirected<T, N> *) noexcept;

//=====================================
template <typename T, std::size_t N>
bool
remove_edge(Undirected<T, N> &, Undirected<T, N> *) noexcept;

//=====================================
template <typename T, std::size_t N>
bool
is_adjacent(const Undirected<T, N> &, const Undirected<T, N> &) noexcept;

//=====================================
template <typename T, std::size_t N>
template <typename A>
Undirected<T, N>::Undirected(A &&arg) noexcept
    : value(std::forward<A>(arg)) {
}

template <typename T, std::size_t N>
Undirected<T, N>::~Undirected() noexcept {
  printf("dtor[%d]", value);
  while (!is_empty(edges)) {
    std::size_t last = edges.length - 1;
    auto res = remove_edge(*this, edges[last].ptr);
    assertx(res);
  }
  printf("-end[%d]\n", value);
}

//=====================================
template <typename T, std::size_t N, typename A>
Undirected<T, N> *
add_vertex(Undirected<T, N> &self, A &&arg) noexcept {
  if (!is_full(self.edges)) {
    auto edge = new Undirected<T, N>(std::forward<A>(arg));
    if (edge) {
      {
        auto res = bin_insert(self.edges, Wrapper<T, N>(edge));
        res->owner = true;
        assertx(res);
      }
      {
        auto res = bin_insert(edge->edges, Wrapper<T, N>(&self));
        assertx(res);
      }
    }
    return edge;
  }
  return nullptr;
}

template <typename T, std::size_t N>
bool
add_edge(Undirected<T, N> &self, Undirected<T, N> *edge) noexcept {
  if (!is_full(self.edges) && !is_full(edge->edges)) {
    {
      auto res = bin_insert(self.edges, Wrapper<T, N>(edge));
      assertx(res);
      assertx(bin_search(self.edges, Wrapper<T, N>(edge)));
    }
    {
      auto res = bin_insert(edge->edges, Wrapper<T, N>(&self));
      assertx(res);
      assertx(bin_search(edge->edges, Wrapper<T, N>(&self)));
    }
    return true;
  }
  return false;
}
//=====================================
namespace impl {
template <typename T, std::size_t N>
bool
change_owner(Wrapper<T, N> &subject, Undirected<T, N> &owner) {
  auto &subject_edges = subject.ptr->edges;
  for (std::size_t i = 0; i < length(subject_edges); ++i) {
    auto &current = subject_edges[i];
    auto cur_ptr = current.ptr;
    if (cur_ptr != &owner) {
      auto s = bin_search(cur_ptr->edges, subject);
      assertx(s);
      if (s) {
        assertx(s->owner == false);
        s->owner = true;
        return true;
      }
    }
  }
  return false;
}

} // namespace impl

template <typename T, std::size_t N>
bool
remove_edge(Undirected<T, N> &self, Undirected<T, N> *edge) noexcept {
  assertx(edge);
  // TODO
  // if self.is_owner(edge)
  //    if current has more than 1 edge
  //    then change owner to that.

  auto res = bin_search(self.edges, Wrapper<T, N>(edge));
  if (res) {
    auto &the_edge = *res;
    if (the_edge.owner) {
      impl::change_owner(the_edge, self);
    }

    if (bin_remove(self.edges, the_edge)) {
      bool res = bin_remove(edge->edges, Wrapper<T, N>(&self));
      // assertx(res);

      return true;
    }
  }

  return false;
}
//=====================================
template <typename T, std::size_t N>
bool
is_adjacent(const Undirected<T, N> &self,
            const Undirected<T, N> &edge) noexcept {
  if (bin_search(self.edges, Wrapper<T, N>(&edge))) {
    assertx(bin_search(edge.edges, Wrapper<T, N>(&self)));
    return true;
  }
  return false;
}

//=====================================

} // namespace graph

#endif
