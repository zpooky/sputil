#ifndef SP_UTIL_GRAPH_DIJKSTRA_H
#define SP_UTIL_GRAPH_DIJKSTRA_H

#include <graph/Undirected.h>

// https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Pseudocode
namespace dijkstra {
//=====================================
template <typename T, std::size_t N>
void
shortest_path(graph::Undirected<T, N> *from,
              graph::Undirected<T, N> *to) noexcept;
// invert weight

//=====================================
template <typename T, std::size_t N>
void
longest_path(graph::Undirected<T, N> *from,
             graph::Undirected<T, N> *to) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {}

// void
// shortest_path(graph::Undirected *from, graph::Undirected *to) noexcept {
// }
//
// //=====================================
// void
// longest_path(graph::Undirected *from, graph::Undirected *to) noexcept {
// }

//=====================================
} // namespace dijkstra

#endif
