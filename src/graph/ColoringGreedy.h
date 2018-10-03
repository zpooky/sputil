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
template <typename T, typename W>
bool
color_greedy(Vertex<T, W> &,
             sp::HashMapProbing<Vertex<T, W> *, int> &) noexcept;

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

template <typename T, typename W>
bool
color_greedy(Vertex<T, W> &graph,
             sp::HashMapProbing<Vertex<T, W> *, int> &result) noexcept {
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
}
