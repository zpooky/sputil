#include "gtest/gtest.h"
#include <stack/HeapStack.h>
#include <stack/Stack.h>

TEST(StackTest, test_empty) {
  sp::Stack<int> stack;
  ASSERT_TRUE(sp::push(stack, 1) == nullptr);
  ASSERT_TRUE(sp::peek(stack) == nullptr);
  int out = 9999;
  ASSERT_FALSE(sp::pop(stack, out));
  ASSERT_EQ(out, 9999);
}

TEST(StackTest, test) {
  constexpr std::size_t size = 10;
  int buffer[size];
  sp::Stack<int> stack(buffer, size);
  ASSERT_TRUE(sp::peek(stack) == nullptr);
  {
    int out = 9999;
    ASSERT_FALSE(sp::pop(stack, out));
    ASSERT_EQ(out, 9999);
  }
  for (std::size_t i = 0; i < size; ++i) {
    int *res = sp::push(stack, int(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, int(i));
  }
  {
    int cmp = size;
    for_each(stack, [&cmp](const auto &c) { /**/
                                            --cmp;
                                            ASSERT_EQ(cmp, c);
    });
    ASSERT_EQ(0, cmp);
  }
  {
    int *res = sp::push(stack, 99);
    ASSERT_FALSE(res);
  }
}

TEST(HeapStackTest, test) {
  sp::HeapStack<int> stack;
  ASSERT_EQ(std::size_t(0), length(stack));
  ASSERT_TRUE(is_empty(stack));
  ASSERT_FALSE(is_full(stack));
  ASSERT_EQ(nullptr, peek(stack));
  int out = 9999;
  ASSERT_FALSE(pop(stack, out));
  ASSERT_EQ(out, 9999);
  for_each(stack, [](const auto &) { assert(false); });

  constexpr int target = 4 * 1024;
  for (int i = 0; i < target; ++i) {
    {
      int cmp = i;
      for_each(stack, [&cmp](const auto &c) { /**/
                                              --cmp;
                                              ASSERT_EQ(cmp, c);
      });
      ASSERT_EQ(0, cmp);
    }
    ASSERT_EQ(std::size_t(i), length(stack));

    int *ins = push(stack, i);
    ASSERT_TRUE(ins);
    ASSERT_EQ(*ins, i);

    ASSERT_FALSE(is_empty(stack));
    ASSERT_FALSE(is_full(stack));
    ASSERT_EQ(ins, peek(stack));
  }

  int it = target;
  while (!is_empty(stack)) {
    ASSERT_EQ(std::size_t(it), length(stack));
    --it;

    int *pe = peek(stack);
    ASSERT_TRUE(pe);
    ASSERT_EQ(it, *pe);

    int out = 9999999;
    ASSERT_TRUE(pop(stack, out));
    ASSERT_EQ(it, out);
  }
  ASSERT_EQ(0, it);
  for_each(stack, [](const auto &) { assert(false); });
}
