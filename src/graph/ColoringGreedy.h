#include <graph/graph2.h>

/*
 * https://compscicenter.ru/media/course_class_attachments/Thore_Husfeldt_Graph_colouring_algorithms.pdf
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
template <typename T, typename W>
bool
color_greedy(Vertex<T, W> &) noexcept;

//=====================================
//====Implementation===================
//=====================================
template <typename T, typename W>
bool
color_greedy(Vertex<T, W> &) noexcept {
  return true;
}

//=====================================
}
