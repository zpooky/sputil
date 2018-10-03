#include "sudoku.h"
#include <collection/Array.h>
#include <cstddef>
#include <graph/ColoringGreedy.h>
#include <graph/graph2.h>
#include <util/assert.h>

namespace np {
//=====================================
Sudoku::Sudoku() noexcept
    : board{0} {
}

//=====================================
bool
geneate(Sudoku &) noexcept {
  return true;
}

//=====================================
bool
solve(Sudoku &) noexcept {
  return false;
}

//=====================================
namespace graph {

using CVtx = ::graph::Vertex<std::size_t>;

static bool
link_graph(CVtx *(edges)[Sudoku::dimensions]) noexcept {
  constexpr std::size_t dimensions = Sudoku::dimensions;

  for (std::size_t i = 0; i < dimensions; ++i) {
    for (std::size_t a = 0; a < dimensions; ++a) {

      if (i != a) {
        add_edge(*edges[i], 0, edges[a]);
      }
    } // for
  }   // for

  return true;
}

static bool
link_horizontal_graph(CVtx *const begin, CVtx *const end) noexcept {
  for (CVtx *it = begin; it != end; ++it) {
    for (CVtx *it2 = begin; it2 != end; ++it2) {

      if (it != it2) {
        if (!add_edge(*it, 0, it2)) {
          assertx(false);
          return false;
        }
      }
    } // for
  }   // for

  return true;
}

static bool
build_graph(sp::UinDynamicArray<CVtx> &board, Sudoku &puzzle,
            sp::HashMapProbing<CVtx *, int> &result) noexcept {
  constexpr std::size_t dimensions = Sudoku::dimensions;

  {
    /* Set the id of each cell */
    std::size_t id = 0;
    for (std::size_t x = 0; x < Sudoku::dimensions; ++x) {
      for (std::size_t y = 0; y < Sudoku::dimensions; ++y) {

        CVtx *const res = insert(board, id++);
        if (!res) {
          assertx(false);
          return false;
        }

        const std::uint8_t value = puzzle.board[x][y];
        if (value != 0) {
          /* Setup the initial prefilled values into the colored result */
          assertx_n(insert(result, res, value));
        }
      }
    }
  }

  /* Horizontal */
  for (std::size_t i = 0; i < length(board); i += dimensions) {
    CVtx *const begin = board.data() + i;
    CVtx *const end = begin + dimensions;

    if (!link_horizontal_graph(begin, end)) {
      return false;
    }
  }

  /* Vertical */
  for (std::size_t i = 0; i < dimensions; ++i) {
    CVtx *vertical[dimensions] = {nullptr};
    std::size_t vidx = 0;
    for (std::size_t a = i; a < length(board); a += dimensions) {
      vertical[vidx] = get(board, a);
      assertx(vertical[vidx]);

      vidx++;
    }
    assertxs(vidx == dimensions, vidx);

    if (!link_graph(vertical)) {
      return false;
    }
  }

  /* Box */
  std::size_t l = 0;
  std::size_t game[dimensions][dimensions] = {0};
  std::size_t *const g = (std::size_t *)&game;

  while (l < length(board)) {
    /* A row is 3 cells high */
    for (std::size_t row = 0; row < 3; ++row) {
      CVtx *cells[dimensions] = {nullptr};
      std::size_t vidx = 0;

      for (std::size_t x = 0; x < 3; ++x) {
        for (std::size_t y = 0; y < 3; ++y) {

          const std::size_t idx = l + y + (x * dimensions);
          cells[vidx] = get(board, idx);

          assertxs(cells[vidx], idx, l, y, x);
          ++vidx;
          assertxs(*(g + idx) == 0, *(g + idx), idx, l, y, x);
          *(g + idx) = idx;
        } // for y
      }   // for x

      l += 3;
      assertxs(vidx == dimensions, vidx);

      if (!link_graph(cells)) {
        return false;
      }

      for (std::size_t x = 0; x < dimensions; ++x) {
        printf("|");
        for (std::size_t y = 0; y < dimensions; ++y) {
          printf("%02zu|", game[x][y]);
        }
        printf("\n");
      }
      printf("\n");

    } // for row

    /* Skip the 2 following lines */
    l += (dimensions * 2);
  } // while

  return true;
}

static void
copy_result(Sudoku &game, sp::UinDynamicArray<CVtx> &cells,
            sp::HashMapProbing<CVtx *, int> &result) noexcept {

  CVtx *it = cells.data();
  for (std::size_t x = 0; x < Sudoku::dimensions; ++x) {
    for (std::size_t y = 0; y < Sudoku::dimensions; ++y) {
      int *const current = lookup(result, it);
      assertx(current);

      game.board[x][y] = *current;
      it++;
    }
  }
}

bool
solve(Sudoku &game) noexcept {
  constexpr std::size_t N = Sudoku::dimensions * Sudoku::dimensions;
  sp::UinDynamicArray<CVtx> cells(N);
  sp::HashMapProbing<CVtx *, int> result;

  if (!build_graph(cells, game, result)) {
    return false;
  }

  CVtx *const root = get(cells, 0);
  assertx(root);

  if (!::graph::color_greedy(*root, result)) {
    return false;
  }

  copy_result(game, cells, result);

  return true;
}
}

//=====================================
static bool
in_range(std::uint8_t val, std::uint8_t start, std::uint8_t end) noexcept {
  return val >= start && val < end;
}

bool
is_solved(const Sudoku &game) noexcept {
  constexpr std::size_t dimensions = Sudoku::dimensions;
  auto &board = game.board;

  /* Horizontal */
  for (std::size_t x = 0; x < dimensions; ++x) {
    bool line[dimensions] = {false};
    for (std::size_t y = 0; y < dimensions; ++y) {

      const std::uint8_t current = board[x][y];
      if (!in_range(current, 1, 10)) {
        return false;
      }
      const std::size_t idx = current - 1;
      if (line[idx]) {
        return false;
      }

      line[idx] = true;
    }
  }

  /* Vertical */
  for (std::size_t y = 0; y < dimensions; ++y) {
    bool line[dimensions] = {false};
    for (std::size_t x = 0; x < dimensions; ++x) {

      const std::uint8_t current = board[x][y];
      if (!in_range(current, 1, 10)) {
        return false;
      }
      const std::size_t idx = current - 1;
      if (line[idx]) {
        return false;
      }

      line[idx] = true;
    }
  }

  return true;
}

//=====================================
void
print(const Sudoku &game) noexcept {
  for (std::size_t x = 0; x < Sudoku::dimensions; ++x) {
    printf("|");
    for (std::size_t y = 0; y < Sudoku::dimensions; ++y) {
      printf("%02d|", game.board[x][y]);
    }
    printf("\n");
  }
}
}
