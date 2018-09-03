#include <gtest/gtest.h>
#include <np/nqueens.h>

TEST(nqueensTest, test) {
  auto f = [](const auto &board, void *) {
    //
    print(board);
    return;
  };

  ASSERT_EQ(1, np::nqueens(1, f, nullptr));
  ASSERT_EQ(0, np::nqueens(2, f, nullptr));

  ASSERT_EQ(2, np::nqueens(4, f, nullptr));
}
