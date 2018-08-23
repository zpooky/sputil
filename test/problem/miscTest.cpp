#include <cstdio>
#include <gtest/gtest.h>
#include <problem/misc_problem.h>

TEST(misc_problemTest, test) {
  std::uint32_t pool[] = {1, 3, 5};
  for (std::uint32_t N = 1; N <= 30; ++N) {
    auto one = sp::represent(N, pool, 3);
    auto two = sp::dp::represent(N, pool, 3);
    ASSERT_EQ(one, two);
    printf("N:%zu perm: %zu\n", N, one);
  }
}

TEST(misc_problemTest, test_60) {
  std::uint32_t pool[] = {1, 3, 5};
  std::uint32_t N = 60;
  // ASSERT_EQ(8, sp::represent(N, pool, 3));
  ASSERT_EQ(294823913505, sp::dp::represent(N, pool, 3));
  printf("%u\n", ~std::uint32_t(0));
}

TEST(misc_problemTest, test_dice_throw) {
  ASSERT_EQ(0, sp::dice_throw(/*target*/ 1, /*dice*/ 4, /*faces*/ 2));
  ASSERT_EQ(2, sp::dice_throw(/*target*/ 3, /*dice*/ 2, /*faces*/ 2));
  ASSERT_EQ(21, sp::dice_throw(/*trget*/ 8, /*dice*/ 6, /*faces*/ 3));
  ASSERT_EQ(4, sp::dice_throw(/*target*/ 5, /*dice*/ 4, /*faces*/ 2));
  ASSERT_EQ(4, sp::dice_throw(/*target*/ 5, /*dice*/ 4, /*faces*/ 3));
}

TEST(misc_problemTest, test_subsets) {
  sp::subsets(0, 3, nullptr, [](std::uint32_t *b, std::size_t l, void *) {
    /**/
    for (std::size_t i = 0; i < l; ++i) {
      printf("%d -> ", b[i]);
    }
    printf("null\n");
  });
}

TEST(misc_problemTest, test_permutations) {
  sp::permutations(0, 3, nullptr, [](std::uint32_t *b, std::size_t l, void *) {
    /**/
    for (std::size_t i = 0; i < l; ++i) {
      printf("%d -> ", b[i]);
    }
    printf("null\n");
  });
}

TEST(misc_problemTest, test_permutations_repating) {
  sp::permutations_repeating(0, 3, nullptr,
                             [](std::uint32_t *b, std::size_t l, void *) {
                               /**/
                               for (std::size_t i = 0; i < l; ++i) {
                                 printf("%d -> ", b[i]);
                               }
                               printf("null\n");
                             });
}
