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
  operator<(const SPTmp &o) const noexcept {
    return weight < o.weight;
  }
};
}

template <typename T>
bool
shortest_path2(Vertex<T, int> *from, Vertex<T, int> *to) noexcept {
  using Vtx = Vertex<T, int>;

  int weight = 0;

  sp::HashSet<Vtx *, impl::hash_vtx<T>> explored;
  heap::StaticMinBinary<impl::SPTmp<T>, 1024> frontier; // TODO
  sp::HashMap<Vtx *, Vtx *, impl::hash_vtx<T>> previous;
  sp::LinkedList<Vtx *> path;

  {
    impl::SPTmp<T> in(from, 0);
    if (!insert(frontier, in)) {
      return false;
    }
  }

  while (!is_empty(frontier)) {
    impl::SPTmp<T> n;
    bool res = take_head(frontier, n);
    assertx(res);
    assertx(n.vtx);

    if (n.vtx == to) {
      weight = n.weight;

      if (from != to) {
        if (!push_back(path, n.vtx)) {
          return false;
        }
      }

      break;
    }

    if (!insert(explored, n.vtx)) {
      return false;
    }

    for (std::size_t i = 0; i < length(n.vtx->edges); ++i) {
      Edge<T, int> &cur_edge = n.vtx->edges[i];
      if (!contains(explored, cur_edge.target)) {
        const int new_weight = n.weight + cur_edge.weight;

        // TODO should supply a compare function the prio does not uniquely
        // identify a entry
        impl::SPTmp<T> *res = lookup(frontier, cur_edge.target);
        if (!res) {
          if (!insert(previous, cur_edge.target, /* <- */ n.vtx)) {
            return false;
          }

          impl::SPTmp<T> in(cur_edge.target, new_weight);
          res = insert(frontier, in);
          if (!res) {
            return false;
          }
        }

        if (new_weight < res->weight) {
          if (!insert(previous, cur_edge.target, /* <- */ n.vtx)) {
            return false;
          }

          res->weight = new_weight;
          res = decrease_key(frontier, res);
          assertx(res);
        }
      }

    } // for

  } // while

  if (!push_back(path, from)) {
    return false;
  }

  // TODO
  // sp::rec::reverse(path);
  // int cmp_weight = 0;
  // cmp_weight = sp::reduce(path, cmp_weight, [](int acum, impl::SPTmp<T> &cur)
  // {
  //   return cur.weight + acum;
  // });
  // assertxs(cmp_weight == weight, cmp_weight, weight);

  return true;
}

} // namespace dijkstra
}

#endif
