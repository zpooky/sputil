#include "knights_tour.h"
#include <util/assert.h>

namespace np {

static std::size_t
knights_tour(sp::Matrix<int> &board, const int x, const int y,
             const unsigned int move, knights_tour_cb cb,
             void *closure) noexcept {
  const std::size_t done = (board.height * board.width);
  if (move == done) {
    cb(board, closure);
    return 1;
  }

  std::size_t result = 0;

  constexpr std::size_t moves = 8;
  static const int cord[moves][2] = {
      /*        { x,  y}, { x,  y} */
      /*  up:*/ {+2, -1}, {+2, +1}, //
      /*right*/ {+1, +2}, {-1, +2}, //
      /* down*/ {-2, +1}, {-2, -1}, //
      /* left*/ {+1, -2}, {-1, -2}  //
  };

  for (std::size_t i = 0; i < moves; ++i) {
    const int x_move = x + cord[i][0];
    const int y_move = y + cord[i][1];

    if (x_move >= 0 && x_move < board.height) {
      if (y_move >= 0 && y_move < board.width) {
        if (board[x_move][y_move] == -1) {
          board[x_move][y_move] = move;
          result += knights_tour(board, x_move, y_move, move + 1, cb, closure);
          board[x_move][y_move] = -1;
        }
      }
    }
  }

  return result;
}

std::size_t
knights_tour(std::size_t n, knights_tour_cb cb, void *closure) noexcept {
  assertxs(n > 0, n);
  printf("knights_tour(n[%zu])\n", n);

  sp::Matrix<int> board(n, n);
  init(board, -1);

  int x = 0;
  int y = 0;
  int move = 0;
  board[x][y] = move;
  auto result = knights_tour(board, x, y, move + 1, cb, closure);
  board[x][y] = -1;

  return result;
}
}
