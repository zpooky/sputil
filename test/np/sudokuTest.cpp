#include <cstring>
#include <gtest/gtest.h>
#include <np/sudoku.h>

TEST(sudokuTest, test) {
  const char *raw = "5300700006001950000980000608000600034008030017000200060600"
                    "00280000419005000080079";
  ASSERT_EQ(std::strlen(raw), np::Sudoku::dimensions * np::Sudoku::dimensions);
  np::Sudoku puzzle;

  std::size_t it = 0;
  for (std::size_t x = 0; x < np::Sudoku::dimensions; ++x) {
    for (std::size_t y = 0; y < np::Sudoku::dimensions; ++y) {
      puzzle.board[x][y] = std::uint8_t(raw[it++] - '0');
    }
  }
  printf("\nbefore:\n");
  print(puzzle);
  printf("\n");

  ASSERT_TRUE(geneate(puzzle));
  ASSERT_TRUE(np::graph::solve(puzzle));

  printf("\nafter:\n");
  print(puzzle);
  printf("\n");

  ASSERT_TRUE(is_solved(puzzle));
}
