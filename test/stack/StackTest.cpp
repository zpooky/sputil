#include "gtest/gtest.h"
#include <stack/Stack.h>

template<>
void swap(int&o,int&t) noexcept {
  o = 1;
  t = 2;
}

TEST(StackTest, test_empty) {
  sp::Stack<int> stack;
  ASSERT_TRUE(sp::push(stack,1) == nullptr);
  ASSERT_TRUE(sp::peek(stack) == nullptr);
  int out = 9999;
  ASSERT_FALSE(sp::pop(stack,out));
  ASSERT_EQ(out,9999);
}

TEST(StackTest, test) {
  constexpr std::size_t size = 10;
  int buffer[size];
  sp::Stack<int> stack(buffer,size);
  ASSERT_TRUE(sp::peek(stack) == nullptr);
  {
    int out = 9999;
    ASSERT_FALSE(sp::pop(stack,out));
    ASSERT_EQ(out,9999);
  }
  for(std::size_t i=0;i<size;++i){
    int* res = sp::push(stack,int(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(*res,int(i));
  }
  {
    int* res = sp::push(stack,99);
    ASSERT_FALSE(res);
  }
}
