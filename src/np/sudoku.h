#ifndef SP_NP_SUDOKU_H
#define SP_NP_SUDOKU_H

#include <cstddef>
#include <cstdint>

namespace np {
//=====================================
struct Sudoku {
  static constexpr std::size_t dimensions = 9;

  std::uint8_t board[dimensions][dimensions];
  Sudoku() noexcept;
};

//=====================================
bool
geneate(Sudoku &) noexcept;

//=====================================

/* Backtracking */
bool
solve(Sudoku &) noexcept;

//=====================================
namespace graph {
bool
solve(Sudoku &) noexcept;
}

//=====================================
bool
is_solved(const Sudoku &) noexcept;

//=====================================
void
print(const Sudoku &) noexcept;
}

#endif
