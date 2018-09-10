#include "knights_tour.h"
#include <graph/graph2.h>
#include <list/LinkedList.h>
#include <sort/introsort.h>
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

struct WarnsdorffsHeuristic {
  bool
  operator()(const Vtx *lhs, const Vtx *rhs) const noexcept {
    assertx(lhs);
    assertx(rhs);
    ssize_t l = length(lhs->edges) - length(rhs->edges);

    return l < 0;
  }
};

static void
print_board(const sp::DynamicStack<const Vtx *> &path) noexcept {
  const int CORD = 5;
  int id = CORD * CORD;

  int board[CORD][CORD];
  int *bp = (int *)&board;
  memset(bp, char(-1), sizeof(board));

  for_each(path, [&id, &bp](const Vtx *current) {
    assertx(current);
    /**/
    *(bp + current->value) = --id;
  });

  for (int x = 0; x < CORD; ++x) {
    for (int y = 0; y < CORD; ++y) {
      printf("%02d|", board[x][y]);
    }
    printf("\n");
  }
  printf("\n");
}

template <typename H, typename Eq>
static std::size_t
traverse(const Vtx &start, sp::HashSet<const Vtx *, H, Eq> &visited,
         sp::DynamicStack<const Vtx *> &path, std::size_t done,
         knights_tour_cb cb, void *closure) noexcept {
  // printf("length(path): %zu\n", length(path));
  if (length(path) == done) {
    // print_board(path);
    return 1;
  }

  sp::DynamicArray<Vtx *> arr(decltype(start.edges)::capacity);
  for (std::size_t i = 0; i < length(start.edges); ++i) {
    auto &edge = start.edges[i];

    Vtx **res = insert(arr, edge.target);
    assertx(res);
    assertx(*res);
  }

  // Sort path edges accouring to its edges where less is highly priority
  sp::rec::introsort<Vtx *, WarnsdorffsHeuristic>(arr.data(), length(arr));
  assertx_f({
    if (length(arr) > 0) {
      std::size_t w = length(arr[0]->edges);
      for (std::size_t i = 0; i < length(arr); ++i) {
        assertxs(w <= length(arr[i]->edges), w, length(arr[i]->edges));
        w = length(arr[i]->edges);
      }
    }
  });

  std::size_t result = 0;

  for (std::size_t i = 0; i < length(arr); ++i) {
    const Vtx *const current = arr[i];

    if (!contains(visited, current)) {
      assertx_n(insert(visited, current));
      assertx_n(push(path, current));

      result += traverse(*current, visited, path, done, cb, closure);

      assertx_n(remove(visited, current));
      const Vtx *pop_out = nullptr;
      assertx_n(pop(path, pop_out));
    }
  }

  return result;
}

std::size_t
knights_tour(std::size_t N, knights_tour_cb cb, void *closure) noexcept {
  if (N == 0) {
    assertx(false);
    return 0;
  }

  sp::LinkedList<Vtx> nodes;
  if (!build_graph(nodes, N)) {
    assertx(false);
    return 0;
  }

  sp::DynamicStack<const Vtx *> path;

  if (!is_empty(nodes)) {
    const Vtx *const start = get(nodes, 0);
    assertx(start);
    assertxs(start->value == 0, start->value);

    sp::HashSet<const Vtx *, VtxHash, VtxEquality> visited;

    assertx_n(insert(visited, start));
    assertx_n(push(path, start));

    return traverse(*start, visited, path, (N * N), cb, closure);
  }

  return 0;
}
}
//=====================================
}
