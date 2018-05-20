#ifndef SP_UTIL_GRAPH_DIJKSTRA_H
#define SP_UTIL_GRAPH_DIJKSTRA_H

#include <graph/Directed.h>

// https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Pseudocode
namespace dijkstra {
//=====================================
template <typename T, std::size_t N>
void
shortest_path(graph::Directed<T, N> *from, graph::Directed<T, N> *to) noexcept;
// invert weight

//=====================================
template <typename T, std::size_t N>
void
longest_path(graph::Directed<T, N> *from, graph::Directed<T, N> *to) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
/**/
}

//=====================================
} // namespace dijkstra

#endif
