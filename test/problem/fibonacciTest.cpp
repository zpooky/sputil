#include <gtest/gtest.h>
#include <problem/fibonacci.h>

TEST(fibonacciTest, test) {
  ASSERT_EQ(0, sp::fibonacci(0));
  ASSERT_EQ(1, sp::fibonacci(1));
  ASSERT_EQ(1, sp::fibonacci(2));
  ASSERT_EQ(2, sp::fibonacci(3));
  ASSERT_EQ(3, sp::fibonacci(4));
  ASSERT_EQ(5, sp::fibonacci(5));
  ASSERT_EQ(8, sp::fibonacci(6));
  ASSERT_EQ(13, sp::fibonacci(7));
  ASSERT_EQ(21, sp::fibonacci(8));
}

TEST(fibonacciTest, test_cmp) {
  for (std::uint32_t i = 0; i < 20; ++i) {
    auto one = sp::fibonacci(i);
    auto two = sp::dp::memo::fibonacci(i);
    ASSERT_EQ(one, two);

    printf("N:%zu fib: %zu\n", i, one);
  }
}
