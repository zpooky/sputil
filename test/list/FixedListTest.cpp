#include "gtest/gtest.h"
#include <list/FixedList.h>

TEST(FixedListTest, test_list) {
  sp::FixedList<int> list;
  ASSERT_EQ(std::size_t(0), list.capacity);
  ASSERT_EQ(std::size_t(0), list.length);

  std::size_t length = 24;
  ASSERT_TRUE(init(list, length));
  ASSERT_EQ(length, list.capacity);
  ASSERT_EQ(std::size_t(0), list.length);

  clear(list);
  ASSERT_EQ(length, list.capacity);
  ASSERT_EQ(std::size_t(0), list.length);

  for (int i = 0; i < int(length); ++i) {
    for (int a = 0; a < i; ++a) {
      int *r = get(list, a);
      ASSERT_TRUE(r);
      ASSERT_EQ(*r, a);
    }

    // printf("get(list, i[%d])\n", i);
    ASSERT_EQ(get(list, i), nullptr);
    {
      int *res = insert(list, i);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }

    int *r = get(list, i);
    ASSERT_TRUE(r);
    ASSERT_EQ(*r, i);

    for (int a = i + 1; a < int(length); ++a) {
      ASSERT_EQ(get(list, a), nullptr);
    }

    {
      int cnt = 0;
      sp::for_each(list, [&cnt](int current) { //
        ASSERT_EQ(current, cnt++);
      });
      ASSERT_EQ(cnt, i + 1);
    }
  }
  ASSERT_TRUE(insert(list, 1) == nullptr);
}

TEST(FixedListTest, test_list_remove) {
  sp::FixedList<int> list;

  const std::size_t length = 1024;
  ASSERT_TRUE(init(list, length));
  ASSERT_EQ(std::size_t(0), list.length);
  ASSERT_EQ(length, list.capacity);

  for (std::size_t i = 0; i < length; ++i) {
    int *res = insert(list, int(i));

    ASSERT_TRUE(res);
    ASSERT_EQ(*res, int(i));
  }

  ASSERT_TRUE(insert(list, 99999) == nullptr);

  for (std::size_t i = 0; i < length; ++i) {
    std::size_t c_lngth = list.length;
    for (std::size_t a = 0; a < c_lngth; ++a) {
      auto *res = get(list, a);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, int(a + i));
    }

    ASSERT_EQ((length - i), list.length);
    ASSERT_TRUE(remove_first(list, [&i](int c) { //
      return c == int(i);

    }));
    ASSERT_EQ((length - i) - 1, list.length);
  }

  ASSERT_EQ(std::size_t(0), list.length);
  ASSERT_EQ(length, list.capacity);

  for (std::size_t i = 0; i < length; ++i) {
    int *res = insert(list, int(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, int(i));
  }

  ASSERT_EQ(length, list.length);
  ASSERT_EQ(length, list.capacity);
}
