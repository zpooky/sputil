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

struct hash_vtxxx {
  std::size_t
  operator()(::graph::Vertex<int> *const &in) const noexcept {
    return in->value;
  }
};

struct VtxEquality {
  bool
  operator()(const Vtx *f, const Vtx *s) const noexcept {
    return f->value == s->value;
  }
};

static int
Id(int x, int y, int N) noexcept {
  return (x * N) + y;
}

static bool
build_graph(sp::LinkedList<Vtx> &nodes, int N) noexcept {
  std::size_t insxx = 0;
  auto factory = [&insxx, &nodes](auto &state, const Vtx *id) {
    insxx++;
    assertx(id);
    // printf("  1.push_back(%d)\n", id->value);
    Vtx *res = push_back(nodes, id->value);
    assertx(res);
    return new (&state.value) Vtx *(res);
  };

  sp::HashSet<Vtx *, hash_vtxxx, VtxEquality> vtxs;

  for (int x = 0; x < N; ++x) {
    for (int y = 0; y < N; ++y) {

      Vtx from_id{Id(x, y, N)};
      printf("%d.\n", from_id.value);
      // printf("from.lookup_compute(%d)\n", from_id.value);
      Vtx **from = lookup_compute(vtxs, &from_id, factory);
      if (!from) {
        assertx(false);
        return false;
      }
      assertxs(insxx == sp::rec::length(vtxs), insxx, sp::rec::length(vtxs));

      for (std::size_t i = 0; i < cords_moves; ++i) {
        const int x_move = x + cords[i][0];
        const int y_move = y + cords[i][1];

        if (in_range(x_move, N) && in_range(y_move, N)) {
          Vtx to_id{Id(x_move, y_move, N)};

          Vtx **to = lookup_compute(vtxs, &to_id, factory);
          if (!to) {
            assertx(false);
            return false;
          }
          assertxs(insxx == sp::rec::length(vtxs), insxx,
                   sp::rec::length(vtxs));

          // TODO undirected since if we can $from -> $to also means $to ->
          // $from
          if (!add_edge(**from, 0, *to)) {
            // assertx(false);
            // return false;
          }
        } // if in_range
      }   // for cords
    }     // for y
  }       // for x

  assertxs(insxx == (N * N), insxx);

  return true;
}

bool
knights_tour(std::size_t n) noexcept {
  sp::LinkedList<Vtx> nodes;
  if (!build_graph(nodes, n)) {
    return false;
  }

  return true;
}
}
//=====================================
}
