#ifndef SP_UTIL_GRAPH_GRAPH_H
#define SP_UTIL_GRAPH_GRAPH_H

#include <collection/Array.h>
#include <utility>

/*
 * http://web.cecs.pdx.edu/~sheard/course/Cs163/Doc/Graphs.html
 *
 */
namespace graph {

/*
 * <->
 */
template <typename T, std::size_t N = 5>
struct Undirected {
  static_assert(N >= 1, "");
  using edges_type = sp::UinStaticArray<Undirected<T, N> *, N>;
  T value;
  edges_type edges;
  /**/
  template <typename A>
  Undirected(A &&) noexcept;
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

//=====================================
template <typename T, std::size_t N, typename A>
Undirected<T, N> *
add_vertex(Undirected<T, N> &self, A &&arg) noexcept {
  if (!is_full(self.edges)) {
    auto edge = new Undirected<T, N>(std::forward<A>(arg));
    // TODO ownership of edge of when to reclaim
    if (edge) {
      {
        auto res = bin_insert(self.edges, edge);
        assertx(res);
      }
      {
        auto res = bin_insert(edge->edges, &self);
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
      auto res = bin_insert(self.edges, edge);
      assertx(res);
    }
    {
      auto res = bin_insert(edge->edges, &self);
      assertx(res);
    }
    return true;
  }
  return false;
}
//=====================================

template <typename T, std::size_t N>
bool
remove_edge(Undirected<T, N> &self, Undirected<T, N> *edge) noexcept {
  if (bin_remove(self.edges, edge)) {
    bool res = bin_remove(edge->edges, &self);
    assertx(res);
    return true;
  }

  return false;
}
//=====================================
template <typename T, std::size_t N>
bool
is_adjacent(const Undirected<T, N> &self,
            const Undirected<T, N> &edge) noexcept {
  if (bin_search(self.edges, &edge)) {
    assertx(bin_search(edge.edges, &self));
    return true;
  }
  return false;
}

//=====================================

}; // namespace graph

#endif
