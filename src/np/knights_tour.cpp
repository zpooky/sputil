#include "knights_tour.h"
#include <graph/graph2.h>
#include <list/LinkedList.h>
#include <util/assert.h>

namespace np {
//=====================================
constexpr std::size_t cords_moves = 8;
static const int cords[cords_moves][2] = {
    /*        { x,  y}, { x,  y} */
    /*  up:*/ {+2, -1}, {+2, +1}, //
    /*right*/ {+1, +2}, {-1, +2}, //
    /* down*/ {-2, +1}, {-2, -1}, //
    /* left*/ {+1, -2}, {-1, -2}  //
};

static bool
in_range(int cord, int limit) noexcept {
  return cord >= 0 && cord < limit;
}

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

  for (std::size_t i = 0; i < cords_moves; ++i) {
    const int x_move = x + cords[i][0];
    const int y_move = y + cords[i][1];

    if (in_range(x_move, board.height)) {
      if (in_range(y_move, board.width)) {
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

//=====================================
namespace graph {
using Vtx = ::graph::Vertex<int>;

template <typename T>
std::size_t
hash_vtx(::graph::Vertex<T> *const &in) noexcept {
  // printf("%p hash = \n");
  return in->value;
}

static bool
build_graph(sp::LinkedList<Vtx> &nodes, int n) noexcept {
  auto compute = [&nodes](const Vtx *v) {
    assertx(v);
    Vtx *res = push_back(nodes, v->value);
    assertx(res);
    // TODO assert res has the smae hash as v in HashSet
    return res;
  };

  sp::HashSet<Vtx *, hash_vtx> vtxs;

  for (int x = 0; x < n; ++x) {
    for (int y = 0; y < n; ++y) {
      for (std::size_t i = 0; i < cords_moves; ++i) {
        const int x_move = x + cords[i][0];
        const int y_move = y + cords[i][1];

        Vtx from_id{x + y};
        Vtx to_id{x_move + y_move};
        Vtx **from = lookup_compute(vtxs, &from_id, compute);
        if (!from) {
          assertx(false);
          return false;
        }

        Vtx **to = lookup_compute(vtxs, &to_id, compute);
        if (!to) {
          assertx(false);
          return false;
        }

        // TODO undirected since if we can $from -> $to also means $to -> $from
        if (!add_edge(**from, 0, *to)) {
          return false;
        }
      }
    }
  }

  return true;
}

bool
knights_tour(std::size_t n) noexcept {
  sp::LinkedList<Vtx> nodes;
  build_graph(nodes, n);

  return false;
}
}
//=====================================
}
