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

struct VtxHash {
  std::size_t
  operator()(int in) const noexcept {
    sp::Hasher<int> h;
    return h(in);
  }

  std::size_t
  operator()(const Vtx *in) const noexcept {
    return operator()(in->value);
  }
};

struct VtxEquality {
  bool
  operator()(const Vtx *f, int s) const noexcept {
    assertx(f);
    return f->value == s;
  }

  bool
  operator()(const Vtx *f, const Vtx *s) const noexcept {
    assertx(f);
    assertx(s);
    return operator()(f, s->value);
  }
};

static int
Id(int x, int y, int N) noexcept {
  return (x * N) + y;
}

static bool
build_graph(sp::LinkedList<Vtx> &nodes, int N) noexcept {
  int insxx = 0;
  auto factory = [&insxx, &nodes](auto &state, int id) {
    insxx++;
    Vtx *res = push_back(nodes, id);
    assertx(res);
    return new (&state.value) Vtx *(res);
  };

  sp::HashSet<Vtx *, VtxHash, VtxEquality> vtxs;

  for (int x = 0; x < N; ++x) {
    for (int y = 0; y < N; ++y) {

      const int from_id = Id(x, y, N);
      printf("%d.\n", from_id);

      Vtx **from = lookup_compute(vtxs, from_id, factory);
      if (!from) {
        assertx(false);
        return false;
      }
      assertxs(insxx == sp::rec::length(vtxs), insxx, sp::rec::length(vtxs));

      for (std::size_t i = 0; i < cords_moves; ++i) {
        const int x_move(x + cords[i][0]);
        const int y_move(y + cords[i][1]);

        if (in_range(x_move, N) && in_range(y_move, N)) {
          const int to_id = Id(x_move, y_move, N);

          Vtx **to = lookup_compute(vtxs, to_id, factory);
          if (!to) {
            assertx(false);
            return false;
          }
          assertxs(insxx == sp::rec::length(vtxs), insxx,
                   sp::rec::length(vtxs));

          ::graph::Undirected<int, int> graph(**from);
          if (!add_edge(graph, 1, *to)) {
            assertx(is_adjacent(**from, **to));
            assertx(is_adjacent(**to, **from));
          }
        } // if in_range
      }   // for cords
    }     // for y
  }       // for x

  assertxs(insxx == (N * N), insxx);

  return true;
}

template <typename H, typename Eq>
static bool
traverse(Vtx &start, sp::HashSet<Vtx *, H, Eq> &visited,
         sp::DynamicStack<Vtx *> &path, std::size_t done) noexcept {
  if (length(path) == done) {
    return true;
  }
  // 1. sort path edges accouring to its edges where less is highly priority
  // 2. for
  /*
   * {
   *   if (!contains(visited, current)) {
   *     Vtx **res = insert(visited, current);
   *     assertx(res);
   *     push(path, current);
   *     if (traverse(current, visited, path, done)) {
   *       return true;
   *     }
   *     pop(path);
   *   }
   * }
   */

  return false;
}

bool
knights_tour(std::size_t N) noexcept {
  if (N == 0) {
    return false;
  }

  sp::LinkedList<Vtx> nodes;
  if (!build_graph(nodes, N)) {
    return false;
  }

  sp::DynamicStack<Vtx *> path;

  if (!is_empty(nodes)) {
    Vtx *const start = get(nodes, 0);
    assertx(start);
    assertxs(start->value == 0, start->value);

    sp::HashSet<Vtx *, VtxHash, VtxEquality> visited;
    if (!traverse(*start, visited, path, (N * N))) {
      return false;
    }
  }

  return true;
}
}
//=====================================
}
