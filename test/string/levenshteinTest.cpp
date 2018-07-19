#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <string/levenshtein.h>

template <typename F>
static void
test_cases(F f) {
  ASSERT_EQ(0, f("test", "test"));
  ASSERT_EQ(1, f("tent", "test"));
  ASSERT_EQ(3, f("kitten", "sitting"));
  ASSERT_EQ(4, f("", "four"));
  ASSERT_EQ(4, f("four", ""));
  ASSERT_EQ(3, f("Saturday", "Sunday"));
}

TEST(levenshteinTest, test) {
  test_cases(ascii::levenshtein);
}

TEST(levenshteinTest, test_mem) {
  test_cases(ascii::dp::levenshtein);
}

template <typename F>
static void
test_speed(F f) {
  constexpr std::size_t cap = 10;
  prng::xorshift32 r(1);
  char first[cap + 1] = {};
  char second[cap + 1] = {};
  memset(first, 0, sizeof(first));
  memset(second, 0, sizeof(first));

  for (std::size_t i = 0; i < cap; ++i) {
    first[i] = (char)uniform_dist(r, 32, 126);
    second[i] = (char)uniform_dist(r, 32, 126);
  }

  printf("___\n");
  ASSERT_EQ(cap, f(first, second));
}

TEST(levenshteinTest, test_speed) {
  ascii::p();
  test_speed(ascii::levenshtein);
  ascii::p(); // 12,146,179
}

TEST(levenshteinTest, test_speed2) {
  ascii::p();
  test_speed(ascii::levenshtein2);
  ascii::p(); //
}

TEST(levenshteinTest, test_mem_speed) {
  test_speed(ascii::dp::levenshtein);
}
