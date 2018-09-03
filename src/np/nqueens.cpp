#include "nqueens.h"

#include <cstdlib>
#include <cstring>

#define Piece_EMPTY char('-')
#define Piece_QUEEN char('Q')

namespace np {
static is_valid_vertical(const sp::Matrix<char> &board, const std::size_t y) {
  std::size_t result = 0;
  for (std::size_t x = 0; x < board.height; ++x) {
    if (board[x][y] == Piece_QUEEN) {
      return false;
    }
  }
  return true;
}

static bool
is_valid_diagonal(const sp::Matrix<char> &board, const std::size_t x,
                  const std::size_t y) {
  std::size_t yi;
  std::size_t xi;
  // |
  // (x,y)
  for (xi = x, yi = y; xi-- > 0 && yi-- > 0;) {
    if (board[xi][yi] == Piece_QUEEN) {
      return false;
    }
    assertx(board[xi][yi] == Piece_EMPTY);
  }

  //     |
  // (x,y)
  for (xi = x, yi = y + 1; xi-- > 0 && yi < board.width; yi++) {
    if (board[xi][yi] == Piece_QUEEN) {
      return false;
    }
    assertx(board[xi][yi] == Piece_EMPTY);
  }

  return true;
}

static std::size_t
nqueens(sp::Matrix<char> &board, const std::size_t x, nqueens_cb cb,
        void *closure) noexcept {
  std::size_t result = 0;
  if (x < board.height) {
    for (std::size_t y = 0; y < board.width; ++y) {
      if (is_valid_diagonal(board, x, y) && is_valid_vertical(board, y)) {
        board[x][y] = Piece_QUEEN;
        result += nqueens(board, x + 1, cb, closure);
        board[x][y] = Piece_EMPTY;
      }
    }
  } else {
    ++result;
    cb(board, closure);
  }

  return result;
}

std::size_t
nqueens(std::size_t n, nqueens_cb cb, void *closure) noexcept {
  sp::Matrix<char> board(n, n);
  init(board, Piece_EMPTY);

  return nqueens(board, 0, cb, closure);
}
}
