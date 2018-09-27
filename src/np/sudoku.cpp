#include "sudoku.h"
#include <collection/Array.h>
#include <cstddef>
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
struct Cell {
  const std::size_t id;
  int value;

  Cell(std::size_t i) noexcept
      : id{i}
      , value{0} {
  }
};

using CVtx = ::graph::Vertex<Cell>;

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
build_graph(sp::UinDynamicArray<CVtx> &board, Sudoku &puzzle) noexcept {
  constexpr std::size_t dimensions = Sudoku::dimensions;

  /* Set the id of each cell */
  for (std::size_t i = 0; i < capacity(board); ++i) {
    if (!insert(board, i)) {
      assertx(false);
      return false;
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
  std::size_t *g = (std::size_t *)&game;

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

static bool
colour_graph(sp::UinDynamicArray<CVtx> &cells) noexcept {
  return false;
}

bool
solve(Sudoku &puzzle) noexcept {
  constexpr std::size_t N = Sudoku::dimensions * Sudoku::dimensions;
  sp::UinDynamicArray<CVtx> cells(N);
  if (!build_graph(cells, puzzle)) {
    return false;
  }

  if (!colour_graph(cells)) {
    return false;
  }

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
    for (std::size_t x = 0; x < dimensions; ++x) {
      // TODO
    }
  }

  return true;
}

//=====================================
}
