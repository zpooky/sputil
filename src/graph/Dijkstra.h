#ifndef SP_UTIL_GRAPH_DIJKSTRA_H
#define SP_UTIL_GRAPH_DIJKSTRA_H

#include <graph/graph2.h>
#include <heap/binary.h>
#include <list/LinkedList.h>
#include <map/HashMap.h>
#include <map/HashSet.h>
#include <util/assert.h>

// https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Pseudocode
namespace graph {
namespace dijkstra {
//=====================================
template <typename T>
bool
shortest_path(Vertex<T, int> *from, Vertex<T, int> *to) noexcept;

//=====================================
template <typename T>
void
longest_path(Vertex<T, int> *from, Vertex<T, int> *to) noexcept;
// invert weight

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename T>
std::size_t
hash_vtx(graph::Vertex<int> *const &in) {
  // printf("%p hash = \n");
  return reinterpret_cast<std::uintptr_t>(in);
}
}

/*
 * template <typename T>
 * bool
 * shortest_path(Vertex<T, int> *from, Vertex<T, int> *to) noexcept {
 *   using Vtx = Vertex<T, int>;
 *
 *   sp::HashSet<Vtx *, impl::hash_vtx<T>> visited;
 *   if (!insert(visited, from)) {
 *     assertx(false);
 *     return false;
 *   }
 *
 *   sp::LinkedListQueue<Vtx *> unvisited;
 *   if (!enqueue(unvisited, from)) {
 *     assertx(false);
 *     return false;
 *   }
 *
 *   sp::HashMap<Vtx *, int, impl::hash_vtx<T>> cost;
 *   if (!insert(cost, from, 0)) {
 *     assertx(false);
 *     return false;
 *   }
 *
 *   while (!is_empty(unvisited)) {
 *     Vtx *current = nullptr;
 *     bool res = dequeue(unvisited, current);
 *     assertx(res);
 *     assertx(current);
 *
 *     const int *cost_to_current = lookup(cost, current);
 *     assertx(cost_to_current);
 *
 *     for (std::size_t i = 0; i < length(current->edges); ++i) {
 *       Edge<T, int> &edge = current->edges[i];
 *       Vtx *target = edge.target;
 *
 *       // calculate balance from current vertex to edge
 *       const int current_cost_to_target = *cost_to_current + edge.weight;
 *
 *       // update cost if necessary
 *       int *existing_cost_to_target = lookup(cost, target);
 *       if (!existing_cost_to_target) {
 *         if (!insert(cost, target, current_cost_to_target)) {
 *           assertx(false);
 *           return false;
 *         }
 *       } else if (*existing_cost_to_target > current_cost_to_target) {
 *         *existing_cost_to_target = current_cost_to_target;
 *       }
 *
 *       // enqueu for visit if not already visited
 *       if (!contains(visited, target)) {
 *         if (!insert(visited, target)) {
 *           assertx(false);
 *           return false;
 *         }
 *         if (!enqueue(unvisited, target)) {
 *           assertx(false);
 *           return false;
 *         }
 *       }
 *     }
 *   }
 *
 *   {
 *     int *cost_to = lookup(cost, to);
 *     assertx(cost_to);
 *     printf("cost: %d\n", *cost_to);
 *   }
 *
 *   return true;
 * }
 */

//=====================================
namespace impl {
template <typename T>
struct SPTmp {
  /* $vtx is a node in the graph */
  Vertex<T, int> *vtx;
  /* $weight is the lowest(fastest) known cost $from to $vtx */
  int weight;

  SPTmp(Vertex<T, int> *v, int c)
      : vtx{v}
      , weight{c} {
  }

  SPTmp()
      : vtx{nullptr}
      , weight{0} {
  }

  bool
  operator<(const SPTmp<T> &o) const noexcept {
    return weight < o.weight;
  }

  bool
  operator==(const Vertex<T, int> *o) const noexcept {
    assertx(o);
    assertx(vtx);
    return vtx == o;
  }

  bool
  operator==(const Vertex<T, int> &o) const noexcept {
    return this->operator==(&o);
  }
};
}

template <typename T>
bool
shortest_path(Vertex<T, int> *const from, Vertex<T, int> *const to) noexcept {
  using Vtx = Vertex<T, int>;

  int result = 0;
  bool found_path = false;
  /* $explored keeps track of handled vertex and vertex present in $frontier.
   */
  sp::HashSet<Vtx *, impl::hash_vtx<T>> explored;
  /* A min-prio queue where shortest path $from to any vertex is present in
   * order. By always handling the shortest path vertex first we can be sure
   * that when we encounter $to we have found its shortest path.
   */
  heap::StaticMinBinary<impl::SPTmp<T>, 1024> frontier; // TODO Dynamic
  /* Fastest know path to $vertex
   * $path = lookup($fastest, $vertex);
   */
  sp::HashMap</*to*/ Vtx *, /*from*/ Vtx *, impl::hash_vtx<T>> fastest;

  if (!insert(frontier, impl::SPTmp<T>(from, 0))) {
    return false;
  }
  if (!insert(explored, from)) {
    assertx(false);
    return false;
  }

  while (!is_empty(frontier)) {
    impl::SPTmp<T> current;
    if (!take_head(frontier, current)) {
      assertx(false);
      return false;
    }
    assertx(current.vtx);

    printf("take_head(weight[%d], Vtx[%d])\n", current.weight,
           current.vtx->value);

    if (current.vtx == to) {
      /* Found $end vertex */
      result = current.weight;
      found_path = true;
      break;
    }

    const int weight_to_current = current.weight;

    for (std::size_t i = 0; i < length(current.vtx->edges); ++i) {
      Edge<T, int> &edge = current.vtx->edges[i];

      if (!contains(explored, edge.target)) {
        if (!insert(explored, edge.target)) {
          assertx(false);
          return false;
        }

        const int edge_weight = weight_to_current + edge.weight;

        /* We are using a sequential search for $edge in $frontier since the
         * vertexes of $frontier are sorted by weight(cost) which in this
         * context we do not know.
         */
        impl::SPTmp<T> *existing =
            sp::n::search(frontier.buffer, frontier.length, edge.target);
        if (!existing) {
          /* Current $edge has not been seen before.
           * - $current path is the quickest.
           * - Prioritize using $current weight($edge_weight) in $frontier.
           */
          if (!insert(fastest, /*to*/ edge.target, /*from*/ current.vtx)) {
            assertx(false);
            return false;
          }

          existing = insert(frontier, impl::SPTmp<T>(edge.target, edge_weight));
          if (!existing) {
            assertx(false);
            return false;
          }
        }

        if (edge_weight < existing->weight) {
          /* $current path is faster than $existing path.
           * - Update to instead use the $current path.
           * - Re-prioritize weight(cost) $edge_weight for $existing in
           *   $frontier.
           */
          if (!insert(fastest, /*to*/ edge.target, /*from*/ current.vtx)) {
            assertx(false);
            return false;
          }

          existing->weight = edge_weight;
          existing = decrease_key(frontier, existing);
          assertx(existing);
        }
      }

    } // for

  } // while

  if (!found_path) {
    return false;
  }

  {
    int cmp_weight = 0;
    sp::DynamicStack<Vtx *> path;

    {
      Vtx *priv = to;
      do {
        push(path, priv);

        Vtx **priv_priv = lookup(fastest, priv);
        assertx(priv_priv);
        assertx(*priv_priv);

        {
          auto edge = get_edge(**priv_priv, priv);
          assertx(edge);
          cmp_weight += edge->weight;
        }

        priv = *priv_priv;
      } while (priv != from);

      push(path, from);
    }

    assertxs(cmp_weight == result, cmp_weight, result);

    Vtx *priv = nullptr;
    for_each(path, [&priv](auto &vtx) {
      int cost = -1;
      if (priv) {
        auto edge = get_edge(*priv, vtx);
        assertx(edge);
        cost = edge->weight;
      }
      priv = vtx;
      /**/
      if (cost > 0) {
        printf("[%d]->Vtx(%d)-", cost, vtx->value);
      } else {
        printf("Vtx(%d)-", vtx->value);
      }
    });
    printf(": sum cost[%d]\n", result);
  }

  return true;
}

} // namespace dijkstra
}

#endif
