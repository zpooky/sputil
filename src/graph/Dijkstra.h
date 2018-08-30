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
// 1. Mark all nodes unvisited. Create a set of all the unvisited nodes
// called the unvisited set.

// 2. Assign to every node a tentative distance value: set it to zero for our
// initial node and to infinity for all other nodes. Set the initial node as
// current.

template <typename T>
bool
shortest_path(Vertex<T, int> *from, Vertex<T, int> *to) noexcept {
  using Vtx = Vertex<T, int>;

  sp::HashSet<Vtx *, impl::hash_vtx<T>> visited;
  if (!insert(visited, from)) {
    assertx(false);
    return false;
  }

  sp::LinkedListQueue<Vtx *> unvisited;
  if (!enqueue(unvisited, from)) {
    assertx(false);
    return false;
  }

  sp::HashMap<Vtx *, int, impl::hash_vtx<T>> cost;
  if (!insert(cost, from, 0)) {
    assertx(false);
    return false;
  }

  while (!is_empty(unvisited)) {
    Vtx *current = nullptr;
    bool res = dequeue(unvisited, current);
    assertx(res);
    assertx(current);

    const int *cost_to_current = lookup(cost, current);
    assertx(cost_to_current);

    for (std::size_t i = 0; i < length(current->edges); ++i) {
      Edge<T, int> &edge = current->edges[i];
      Vtx *target = edge.target;

      // calculate balance from current vertex to edge
      const int current_cost_to_target = *cost_to_current + edge.weight;

      // update cost if necessary
      int *existing_cost_to_target = lookup(cost, target);
      if (!existing_cost_to_target) {
        if (!insert(cost, target, current_cost_to_target)) {
          assertx(false);
          return false;
        }
      } else if (*existing_cost_to_target > current_cost_to_target) {
        *existing_cost_to_target = current_cost_to_target;
      }

      // enqueu for visit if not already visited
      if (!contains(visited, target)) {
        if (!insert(visited, target)) {
          assertx(false);
          return false;
        }
        if (!enqueue(unvisited, target)) {
          assertx(false);
          return false;
        }
      }
    }
  }

  {
    int *cost_to = lookup(cost, to);
    assertx(cost_to);
    printf("cost: %d\n", *cost_to);
  }

  return true;
}

//=====================================
namespace impl {
template <typename T>
struct SPTmp {
  Vertex<T, int> *vtx;
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
shortest_path2(Vertex<T, int> *from, Vertex<T, int> *to) noexcept {
  using Vtx = Vertex<T, int>;

  int weight = 0;
  bool found_path = false;

  sp::HashSet<Vtx *, impl::hash_vtx<T>> explored;

  heap::StaticMinBinary<impl::SPTmp<T>, 1024> frontier; // TODO

  /* Fastest know path to $vertex
   * fastest = lookup(previous, $vertex);
   */
  sp::HashMap<Vtx *, Vtx *, impl::hash_vtx<T>> previous;

  if (!insert(frontier, impl::SPTmp<T>(from, 0))) {
    return false;
  }

  while (!is_empty(frontier)) {
    impl::SPTmp<T> current;
    bool res = take_head(frontier, current);
    assertx(res);
    assertx(current.vtx);

    if (current.vtx == to) {
      /* Found end */
      weight = current.weight;
      found_path = true;
      break;
    }

    if (!insert(explored, current.vtx)) {
      assertx(false);
      return false;
    }

    for (std::size_t i = 0; i < length(current.vtx->edges); ++i) {
      Edge<T, int> &edge = current.vtx->edges[i];

      if (!contains(explored, edge.target)) {
        const int new_weight = current.weight + edge.weight;

        impl::SPTmp<T> *res =
            sp::n::search(frontier.buffer, frontier.length, edge.target);
        if (!res) {
          if (!insert(previous, edge.target, /* <- */ current.vtx)) {
            assertx(false);
            return false;
          }

          impl::SPTmp<T> in(edge.target, new_weight);
          res = insert(frontier, in);
          if (!res) {
            assertx(false);
            return false;
          }
        }

        if (new_weight < res->weight) {
          if (!insert(previous, edge.target, /* <- */ current.vtx)) {
            assertx(false);
            return false;
          }

          res->weight = new_weight;
          res = decrease_key(frontier, res);
          assertx(res);
        }
      }

    } // for

  } // while

  assertx(found_path);

  {
    int cmp_weight = 0;
    sp::HeapStack<Vtx *> path;

    {
      Vtx *priv = to;
      do {
        push(path, priv);

        Vtx **priv_priv = lookup(previous, priv);
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

    assertxs(cmp_weight == weight, cmp_weight, weight);

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
    printf(": sum cost[%d]\n", weight);
  }

  return true;
}

} // namespace dijkstra
}

#endif
