#include <gtest/gtest.h>
#include <np/sudoku.h>

TEST(sudokuTest, test) {
  np::Sudoku puzzle;
  ASSERT_TRUE(geneate(puzzle));
  ASSERT_TRUE(np::graph::solve(puzzle));
  ASSERT_TRUE(is_solved(puzzle));
}
