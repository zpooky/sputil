#ifndef SP_UTIL_GRAPH_GRAPH_H
#define SP_UTIL_GRAPH_GRAPH_H

#include <collection/Array.h>
#include <hash/fnv.h>
#include <map/Set.h>
#include <memory/StackPooledAllocator.h>
#include <queue/Queue.h>
#include <stack/HeapStack.h>
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
    uintptr_t first = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t second = reinterpret_cast<uintptr_t>(o);
    return first > second;
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
breadth_first(Undirected<T, N> &, F) noexcept;

template <typename T, std::size_t N, typename F>
void
breadth_first(const Undirected<T, N> &, F) noexcept;

//=====================================
template <typename T, std::size_t N, typename F>
void
all_paths(const Undirected<T, N> &, F) noexcept;

//=====================================
template <typename T, std::size_t N, typename F>
void
depth_first(Undirected<T, N> &, F) noexcept;

template <typename T, std::size_t N, typename F>
void
depth_first(const Undirected<T, N> &, F) noexcept;

//=====================================
// TODO emplace_vertex()
template <typename T, std::size_t N, typename A>
Undirected<T, N> *
add_vertex(Undirected<T, N> &, A &&) noexcept;

template <typename T, std::size_t N>
bool
add_edge(Undirected<T, N> &, Undirected<T, N> *, bool owner = false) noexcept;

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
template <typename T, std::size_t N, typename F>
void
breadth_first(Undirected<T, N> &root, F f) noexcept {
  using blah = Undirected<T, N> *;
  sp::HashSet<blah, fnv_1a::hash<blah>> visited;

  sp::LinkedListQueue<Undirected<T, N> *, sp::StackPooledAllocator> toVisit;

  {
    auto res = enqueue(toVisit, &root);
    assertx(res);
  }
  {
    auto res = insert(visited, &root);
    assertx(res);
  }
  while (!is_empty(toVisit)) {
    Undirected<T, N> *current = nullptr;
    dequeue(toVisit, current);
    assertx(current);
    f(*current);

    for (std::size_t i = 0; i < length(current->edges); ++i) {
      auto &edge = current->edges[i];
      if (insert(visited, edge.ptr)) {
        // insert only succeeds if it does not already contain edge
        auto res = push(toVisit, edge.ptr);
        assertx(res);
      }
    }
  }
}

template <typename T, std::size_t N, typename F>
void
breadth_first(const Undirected<T, N> &self, F f) noexcept {
  Undirected<T, N> &s = (Undirected<T, N> &)self;
  breadth_first(s, [f](const auto &c) { f(c); });
}

//=====================================
template <typename T, std::size_t N, typename F>
void
depth_first(Undirected<T, N> &root, F f) noexcept {
  using blah = Undirected<T, N> *;
  sp::HashSet<blah, fnv_1a::hash<blah>> visited;

  sp::HeapStack<Undirected<T, N> *, sp::StackPooledAllocator> toVisit;

  {
    auto res = push(toVisit, &root);
    assertx(res);
  }
  {
    auto res = insert(visited, &root);
    // assertx(res);
  }
  while (!is_empty(toVisit)) {
    Undirected<T, N> *current = nullptr;
    pop(toVisit, current);
    assertx(current);
    f(*current);

    for (std::size_t i = 0; i < length(current->edges); ++i) {
      auto &edge = current->edges[i];
      if (insert(visited, edge.ptr)) {
        // insert only succeeds if it does not already contain edge
        auto res = push(toVisit, edge.ptr);
        assertx(res);
      }
    }
  }
}

template <typename T, std::size_t N, typename F>
void
depth_first(const Undirected<T, N> &self, F f) noexcept {
  Undirected<T, N> &s = (Undirected<T, N> &)self;
  depth_first(s, [f](const auto &c) { f(c); });
}

//=====================================
template <typename T, std::size_t N, typename A>
Undirected<T, N> *
add_vertex(Undirected<T, N> &self, A &&arg) noexcept {
  if (!is_full(self.edges)) {
    auto edge = new Undirected<T, N>(std::forward<A>(arg));
    if (edge) {
      if (add_edge(self, edge, true)) {
        // debug {
        std::size_t cnt = 0;
        for_each(self.edges, [&cnt, edge](const auto &w) {
          if (w.ptr == edge) {
            ++cnt;
          }
        });
        assertxs(cnt == 1, cnt);
        //}
        return edge;
      }
      // bug! edge is created here therefore it should never be a duplicate
      assertx(false);
      delete edge;
    }
  }

  return nullptr;
}

template <typename T, std::size_t N>
bool
add_edge(Undirected<T, N> &self, Undirected<T, N> *edge, bool owner) noexcept {
  assertx(edge);

  if (&self == edge) {
    // self assignement
    assertx(false);
    return false;
  }

  if (is_full(self.edges)) {
    return false;
  }

  if (is_full(edge->edges)) {
    return false;
  }

  const auto search_res = bin_search(self.edges, Wrapper<T, N>(edge));
  const std::size_t s_edges = length(self.edges);

  // should only fail on duplicate edge
  auto res = bin_insert_unique(self.edges, Wrapper<T, N>(edge));
  if (res) {
    assertx(search_res == nullptr);
    assertxs((s_edges + 1) == length(self.edges), s_edges, length(self.edges));

    res->owner = owner;
    assertx(res->ptr == edge);

    {
      auto res2 = bin_insert_unique(edge->edges, Wrapper<T, N>(&self));
      assertx(res2);
      assertx(res2->ptr == &self);
    }

    return true;
  }
  assertxs(s_edges == length(self.edges), s_edges, length(self.edges));
  assertx(search_res != nullptr);

  return false;
}
//=====================================
namespace impl {
template <typename T, std::size_t N>
bool
change_owner(Wrapper<T, N> &subject, Undirected<T, N> &owner) {
  // printf("change_owner(subject[%p], owner[%p])\n", &subject, &owner);
  auto &subject_edges = subject.ptr->edges;
  for (std::size_t i = 0; i < length(subject_edges); ++i) {
    auto &current = subject_edges[i];
    auto cur_ptr = current.ptr;
    if (cur_ptr != &owner) {
      auto s = bin_search(/*haystack*/ cur_ptr->edges, /*needle*/ subject);
      assertx(s);
      if (s) {
        assertx(!s->owner);
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

  auto res = bin_search(self.edges, Wrapper<T, N>(edge));
  if (res) {
    auto &the_edge = *res;
    if (the_edge.owner) {
      if (impl::change_owner(the_edge, self)) {
        // there where another owner
        the_edge.owner = false;
      }
    }

    // TODO if still owner we should delay the delete until both sides has
    // removed eachother from edges list = bin_take

    // TODO owner <-> owner work
    {
      // remove ourself from edge since we might dtor edge
      bool rres = bin_remove(edge->edges, Wrapper<T, N>(&self));
      assertx(rres);
    }

    {
      bool rres = bin_remove(self.edges, the_edge);
      assertx(rres);
    }
    //
    return true;
    // }
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
