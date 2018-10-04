#include <algorithm>
#include <collection/Array.h>
#include <graph/graph2.h>
#include <map/HashMapProbing.h>
#include <util/assert.h>

/* https://compscicenter.ru/media/course_class_attachments/Thore_Husfeldt_Graph_colouring_algorithms.pdf
 * https://www.geeksforgeeks.org/graph-coloring-set-2-greedy-algorithm/
 * https://martin-thoma.com/vertex-coloring/
 * https://github.com/boostorg/graph/blob/develop/include/boost/graph/sequential_vertex_coloring.hpp
 *
 * https://github.com/boostorg/graph/tree/develop/include/boost/graph
 *
 * # definition
 * http://www.sci.brooklyn.cuny.edu/~amotz/GC-ALGORITHMS/PRESENTATIONS/coloring.pdf
 */
namespace graph {
//=====================================
/* Does not guarantee minimum colors */
template <typename T, typename W, typename H, typename Eq>
bool
color_greedy(Vertex<T, W> &,
             sp::HashMapProbing<Vertex<T, W> *, int, H, Eq> &) noexcept;

//=====================================
/*
 * Backtracking, Recursive, guarantee the minimum colors.
 * TODO configurable max limt
 */
template <typename T, typename W, typename H, typename Eq>
bool
color(Vertex<T, W> &,
      sp::HashMapProbing<Vertex<T, W> *, int, H, Eq> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
static inline int
find_unused(sp::HashSetProbing<int> &taken) {
  int it = 0;

  int *res = nullptr;
  do {
    ++it;
    res = lookup(taken, it);
  } while (res);

  return it;
}

template <typename T, typename W, typename H, typename Eq>
bool
color_greedy(Vertex<T, W> &graph,
             sp::HashMapProbing<Vertex<T, W> *, int, H, Eq> &result) noexcept {
  using Vtx = Vertex<T, W>;
  using Color = int;

  Color k = 0;
  std::size_t len = 0;

  deapth_first(graph, [&result, &len, &k](Vtx &current) {
    ++len;
    int *const pre = lookup(result, &current);
    if (pre) {
      /* Ignore precolored vertex */
      return;
    }

    sp::HashSetProbing<Color> taken;
    for_each_edge(current, [&result, &taken](Edge<T, W> &edge) {
      Color *const reserved = lookup(result, edge.target);
      if (reserved) {
        insert(taken, *reserved);
      }
    });

    const Color c = find_unused(taken);
    k = std::max<Color>(k, c);
    // assertxs(c >= 1 && c <= 9, c); // TODO
    assertx_n(insert(result, &current, c));
  });

  printf("MAX(%d)\n", k);

  assertxs(len == length(result), len, length(result));

  return true;
}

//=====================================
namespace impl {
template <typename F>
static bool
for_wasd(sp::HashSetProbing<int> &taken, F f) noexcept {
  const int limit = 9;

  int it = 0;
  while (it++ < limit) {
    const int *const l = lookup(taken, it);

    if (!l) {
      if (f(it)) {
        return true;
      }
    }
  }

  return false;
}

template <typename T, typename W, typename H, typename Eq>
bool
c(sp::HashSetProbing<Vertex<T, W> *> &visited,
  sp::DynamicStack<Vertex<T, W> *> &stack,
  sp::HashMapProbing<Vertex<T, W> *, int, H, Eq> &result) noexcept {

  using Vtx = Vertex<T, W>;

  Vtx *current = nullptr;
  if (pop(stack, current)) {
    assertxs(current, length(stack));

    sp::DynamicStack<Vtx *> tmp;
    sp::HashSetProbing<int> cur_tkn;
    /* Current visited */
    sp::DynamicStack<Vtx *> cv;

    auto fe = [&result, &cur_tkn, &tmp, &cv, &visited](Edge<T, W> &edge) {
      int *const reserved = lookup(result, edge.target);
      if (reserved) {
        insert(cur_tkn, *reserved);
      }

      if (!lookup(visited, edge.target)) {
        insert(visited, edge.target);

        push(tmp, edge.target);
        push(cv, edge.target);
      }
    };
    for_each_edge(*current, fe);

    const int *const prefill = lookup(result, current);
    bool res = false;

    if (prefill) {
      insert(stack, tmp);
      res = c(visited, stack, result);
    } else {
      auto avail_f = [&result, current, &visited, &stack, &tmp](int color) {
        sp::DynamicStack<Vtx *> junk;
        insert(junk, stack);
        insert(junk, tmp);

        insert(result, current, color);
        if (c(visited, junk, result)) {
          swap(stack, junk);
          return true;
        }
        remove(result, current);

        return false;
      };
      res = for_wasd(cur_tkn, avail_f);
    }

    if (!res) {
      /* Unvisit current since we fail to find valid solution */
      for_each(cv, [&visited](Vtx *edge) {
        /**/
        remove(visited, edge);
      });
    }

    return res;
  }

  return true;
}
} // namespace impl

template <typename T, typename W, typename H, typename Eq>
bool
color(Vertex<T, W> &root,
      sp::HashMapProbing<Vertex<T, W> *, int, H, Eq> &result) noexcept {
  using Vtx = Vertex<T, W>;
  sp::HashSetProbing<Vtx *> visited;

  sp::DynamicStack<Vertex<T, W> *> stack;
  if (!push(stack, &root)) {
    return false;
  }

  if (!insert(visited, &root)) {
    return false;
  }

  return impl::c(visited, stack, result);
}
} // namespace graph
