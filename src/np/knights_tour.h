#ifndef SP_UTIL_NP_KNIGHTS_TOUR_H
#define SP_UTIL_NP_KNIGHTS_TOUR_H

#include <cstddef>
#include <util/Matrix.h>

// TODO not sure if this is np
// TODO add XXX heuristic (just sort x,y canditates according to valid edge
//      count where smaller amount of edges is better)

namespace np {
/* # Knight's Tour
 * Given a $n*$n chess board, the starting position x:0,y:0. Using the
 * chess piece knight(horse) move to every position of the board without vesting
 * the same position twice.
 */
typedef void (*knights_tour_cb)(const sp::Matrix<int> &board, void *);

/* recursive, backtracking
 * Result is a matrix where is entry contains the jump number of the knight:
 */
// TODO not sure if this is working...
std::size_t
knights_tour(std::size_t n, knights_tour_cb, void *closure) noexcept;

namespace graph {
// https://bradfieldcs.com/algos/graphs/knights-tour/
bool
knights_tour(std::size_t n) noexcept;
}
}

#endif