#include <gtest/gtest.h>
#include <problem/tower_of_hanoi.h>

TEST(tower_of_hanoiTest, test) {
  sp::DynamicStack<int> from;
  sp::DynamicStack<int> to;

  for (int i = 1; i < 10; ++i) {
    // for (int i = 1; i < 10; ++i) {
    ASSERT_TRUE(push(from, i));
  }

  sp::tower_of_hanoi(from, to);
}
