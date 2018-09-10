#ifndef SP_UTIL_NP_KNIGHTS_TOUR_H
#define SP_UTIL_NP_KNIGHTS_TOUR_H

#include <cstddef>
#include <util/Matrix.h>

// TODO not sure if this is np

namespace np {
/* # Knight's Tour
 * Given a $n*$n chess board, the starting position x:0,y:0. Using the chess
 * piece knight(horse) move to every position of the board without vesting the
 * same position twice.
 *
 *   _ _
 *    |
 * |__|__|
 * |  |  |
 *   _|_
 *
 * Example solution for 5x5 where `00` is the start position.
 * 00 03 10 15 24
 * 11 16 01 04 09
 * 02 19 06 23 14
 * 17 12 21 08 05
 * 20 07 18 13 22
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

/*
 * Using Warnsdorffs Heuristic where vertex with less connecting edges are
 * prioritized over vertex with more edges. This optimization is useful when we
 * want to find ANY valid solution not all of them. It works by visiting the
 * edges of the board(which has less edges since its on the edge of the
 * board) before visiting the more central positions. This results that we
 * faster discard invalid solutions by first visiting the more harder position
 * before the easier central positions.
 *
 * A edge represents a valid move(for a knight) from a vertex to another vertex.
 * The graph is bidirectional since a valid move from a vertex to another can
 * be reversed by performing the inverse move.
 */
std::size_t
knights_tour(std::size_t n, knights_tour_cb, void *closure) noexcept;
}
}

#endif
