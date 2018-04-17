#include "gtest/gtest.h"
#include <util/permutation.h>

TEST(permutationTest, perm_test2_2) {
  {
    std::size_t res = 0;
    ASSERT_TRUE(sp::rec::permutations("01", 2, [&res](const auto &str) {
      /**/
      printf("%s\n", str);
      ++res;
    }));
    ASSERT_EQ(std::size_t(4), res);
  }
}

TEST(permutationTest, perm_test2_3) {
  {
    std::size_t res = 0;
    ASSERT_TRUE(sp::rec::permutations("01", 2, 3, [&res](const auto &str) {
      /**/
      printf("%s\n", str);
      ++res;
    }));
    ASSERT_EQ(std::size_t(8), res);
  }
}

TEST(permutationTest, perm_test3_3) {
  {
    std::size_t res = 0;
    ASSERT_TRUE(sp::rec::permutations("012", 3, 3, [&res](const auto &str) {
      /**/
      printf("%s\n", str);
      ++res;
    }));
    ASSERT_EQ(std::size_t(27), res);
  }
}

TEST(permutationTest, combination_test) {
  {
    std::size_t res = 0;
    const char *in = "01";
    ASSERT_TRUE(sp::rec::combinations(in, strlen(in), [&res](const auto &str) {
      for_each(str, [](const char *c) {
        /**/
        printf("%c", *c);
      });

      printf("\n");
      /**/
      // printf("%s\n", str);
      ++res;
    }));
    ASSERT_EQ(std::size_t(2), res);
  }
}
