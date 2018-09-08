#ifndef SP_UTIL_NP_NQUEENS_H
#define SP_UTIL_NP_NQUEENS_H

#include <cstddef>
#include <util/Matrix.h>

// TODO not sure if this is np
namespace np {
/* # N-Queen
 * Given $n queens and a $n*$n board calculate all posible position of the $n
 * queens.
 *
 * Q: Queen
 * -: empty
 */

typedef void (*nqueens_cb)(const sp::Matrix<char> &board, void *);

/*
 * Recursive, Backtracking
 */
std::size_t
nqueens(std::size_t n, nqueens_cb, void *) noexcept;

/* TODO additional implementation competetive programmers handbook[p61]
 */
} // namespace np

#endif
