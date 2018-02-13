#include "gtest/gtest.h"
#include <list/FixedList.h>

TEST(FixedListTest, test_list) {
  sp::FixedList<int> list;
  ASSERT_EQ(std::size_t(0), capacity(list));
  ASSERT_EQ(std::size_t(0), length(list));

  std::size_t xlength = 24;
  ASSERT_TRUE(init(list, xlength));
  ASSERT_EQ(xlength, capacity(list));
  ASSERT_EQ(std::size_t(0), length(list));

  clear(list);
  ASSERT_EQ(xlength, capacity(list));
  ASSERT_EQ(std::size_t(0), length(list));

  for (int i = 0; i < int(xlength); ++i) {
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

    for (int a = i + 1; a < int(xlength); ++a) {
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

  const std::size_t xlength = 1024;
  ASSERT_TRUE(init(list, xlength));
  ASSERT_EQ(std::size_t(0), length(list));
  ASSERT_EQ(xlength, capacity(list));

  for (std::size_t i = 0; i < xlength; ++i) {
    int *res = insert(list, int(i));

    ASSERT_TRUE(res);
    ASSERT_EQ(*res, int(i));
  }

  ASSERT_TRUE(insert(list, 99999) == nullptr);

  for (std::size_t i = 0; i < xlength; ++i) {
    std::size_t c_lngth = length(list);
    for (std::size_t a = 0; a < c_lngth; ++a) {
      auto *res = get(list, a);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, int(a + i));
    }

    ASSERT_EQ((xlength - i), length(list));
    ASSERT_TRUE(remove_first(list, [&i](int c) { //
      return c == int(i);

    }));
    ASSERT_EQ((xlength - i), length(list));
  }

  ASSERT_EQ(std::size_t(0), length(list));
  ASSERT_EQ(xlength, capacity(list));

  for (std::size_t i = 0; i < xlength; ++i) {
    int *res = insert(list, int(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, int(i));
  }

  ASSERT_EQ(xlength, length(list));
  ASSERT_EQ(xlength, capacity(list));
}
