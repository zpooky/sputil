#include "gtest/gtest.h"
#include <buffer/CircularBuffer.h>

using namespace sp;

TEST(CircularBufferTest, test) {
  constexpr int cap = 16;
  int b[cap];
  CircularBuffer<int> buffer(b);

  ASSERT_EQ(peek_front(buffer), nullptr);
  ASSERT_EQ(peek_back(buffer), nullptr);
  ASSERT_EQ(get(buffer, std::size_t(0)), nullptr);
  ASSERT_TRUE(is_empty(buffer));

  for (int i = 0; i < cap; ++i) {
    ASSERT_EQ(push_back(buffer, i), i);
    auto *pb = peek_back(buffer);
    ASSERT_TRUE(pb);
    ASSERT_EQ(*pb, i);
    ASSERT_FALSE(is_empty(buffer));
  }
  // printf("read: %zu\nwrite: %zu\nsize: %zu\n",
  //        sp::impl::CircularBuffer::index(buffer.read, buffer.capacity),
  //        sp::impl::CircularBuffer::index(buffer.write, buffer.capacity),
  // length(buffer));
  ASSERT_TRUE(is_full(buffer));
  for (int i = 0; i < cap; ++i) {
    std::size_t idx(i);
    auto *f = get(buffer, idx);
    ASSERT_TRUE(f);
    ASSERT_EQ(*f, i);
  }

  for (int i = 0; i < cap; ++i) {
    auto *pf = peek_front(buffer);
    ASSERT_TRUE(pf);
    ASSERT_EQ(*pf, i);

    int res = 99999;
    ASSERT_TRUE(pop_front(buffer, res));
    ASSERT_EQ(res, i);
  }
  ASSERT_TRUE(is_empty(buffer));
  ASSERT_FALSE(is_full(buffer));
  ASSERT_EQ(std::size_t(0), length(buffer));
}

TEST(CircularBufferTest, wrap) {
  constexpr int cap = 16;
  int b[cap];
  CircularBuffer<int> buffer(b);

  constexpr int wrap_cnt = 1000;
  for (int i = 0; i < wrap_cnt; ++i) {
    ASSERT_EQ(push_back(buffer, i), i);
  }
  ASSERT_TRUE(is_full(buffer));
  ASSERT_TRUE(!is_empty(buffer));

  for (int i = wrap_cnt - cap; i < wrap_cnt; ++i) {
    auto *f = peek_front(buffer);
    ASSERT_TRUE(f);
    ASSERT_EQ(*f, i);

    int res = 999999;
    ASSERT_TRUE(pop_front(buffer, res));
    ASSERT_EQ(res, i);
  }

  ASSERT_TRUE(!is_full(buffer));
  ASSERT_TRUE(is_empty(buffer));
}

TEST(CircularBufferTest, pop_back) {
  constexpr int cap = 16;
  int b[cap];
  CircularBuffer<int> buffer(b);

  for (int a = 0; a < cap; ++a) {
    for (int b = 0; b < cap; ++b) {
      ASSERT_EQ(push_back(buffer, 999), 999);
    }

    {
      for (int i = 0; i < cap; ++i) {
        ASSERT_EQ(push_back(buffer, i), i);
      }
      ASSERT_TRUE(is_full(buffer));
      ASSERT_TRUE(!is_empty(buffer));

      for (int i = cap - 1; i >= 0; --i) {
        auto *pb = peek_back(buffer);
        ASSERT_TRUE(pb);
        ASSERT_EQ(*pb, i);

        int res = 99999;
        ASSERT_TRUE(pop_back(buffer, res));
        // printf("%d\n", res);
        ASSERT_EQ(res, i);
      }

      ASSERT_TRUE(!is_full(buffer));
      ASSERT_TRUE(is_empty(buffer));
    }
  }
}

