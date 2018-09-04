#include <gtest/gtest.h>
#include <np/knights_tour.h>

TEST(knights_tourTest, test) {
  auto f = [](const auto &board, void *) {
    /**/
    // print(board);
  };

  // 1; 0; 0; 0; 1,728; 6,637,920
  ASSERT_EQ(1, np::knights_tour(1, f, nullptr));
  ASSERT_EQ(0, np::knights_tour(2, f, nullptr));
  ASSERT_EQ(0, np::knights_tour(3, f, nullptr));
  ASSERT_EQ(0, np::knights_tour(4, f, nullptr));
  ASSERT_EQ(1728, np::knights_tour(5, f, nullptr));
  ASSERT_EQ(6637920, np::knights_tour(6, f, nullptr));
  ASSERT_EQ(165575218320, np::knights_tour(7, f, nullptr));
}
