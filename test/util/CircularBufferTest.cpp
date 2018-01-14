#include "gtest/gtest.h"
#include <util/CircularBuffer.h>

using namespace sp;

TEST(CircularBufferTest, test) {
  constexpr int cap = 16;
  int b[cap];
  CircularBuffer<int> buffer(b);

  ASSERT_TRUE(is_empty(buffer));
  for (int i = 0; i < cap; ++i) {
    ASSERT_EQ(push_back(buffer, i), i);
    ASSERT_FALSE(is_empty(buffer));
  }
  printf("read: %zu\nwrite: %zu\nsize: %zu\n",
         sp::impl::CircularBuffer::index(buffer.read, buffer.capacity),
         sp::impl::CircularBuffer::index(buffer.write, buffer.capacity),
         length(buffer));
  ASSERT_TRUE(is_full(buffer));

  for (int i = 0; i < cap; ++i) {
    int res = 99999;
    ASSERT_TRUE(pop_front(buffer, res));
    ASSERT_EQ(res, i);
  }
  ASSERT_TRUE(is_empty(buffer));
  ASSERT_FALSE(is_full(buffer));
  ASSERT_EQ(0, length(buffer));

  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQ(push_back(buffer, i), i);
  }
}
