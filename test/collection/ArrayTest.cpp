#include "gtest/gtest.h"
#include <collection/Array.h>
#include <prng/xorshift.h>

TEST(ArrayTest, test_empty) {
  sp::Array<int> a;
  ASSERT_EQ(insert(a, 1), nullptr);
  ASSERT_EQ(get(a, 0), nullptr);
}

TEST(ArrayTest, test) {
  int buffer[10];
  sp::Array<int> a(buffer);
  {
    int *res = insert(a, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 0);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 1);
    ASSERT_FALSE(res);
  }
  sp::Xorshift32 r(1);
  shuffle(r, a);
  {
    int *res = get(a, 0);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 1);
    ASSERT_FALSE(res);
  }
}

struct ArrayTestData {
  void *ptr;
  int i;
  ArrayTestData()
      : ptr(nullptr)
      , i(-1) {
  }

  explicit ArrayTestData(int b)
      : ptr(nullptr)
      , i(b) {
  }
};

static void
swap(ArrayTestData &f, ArrayTestData &s) noexcept {
  sp::swap(f.i, s.i);
}

TEST(ArrayTest, test_data) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> a(buffer);
  sp::Array<ArrayTestData> b;
  swap(a, b);

  for (int i = 0; i < len; ++i) {
    ArrayTestData in(i);
    auto *res = insert(b, in);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }

  sp::Xorshift32 r(1);
  for (int i = 0; i < int(b.length); ++i) {
    auto *c = get(b, std::size_t(i));
    ASSERT_TRUE(c);
    printf("%d,", c->i);
  }
  printf("\n");
  shuffle(r, b);
  for (int i = 0; i < int(b.length); ++i) {
    auto *c = get(b, std::size_t(i));
    ASSERT_TRUE(c);
    printf("%d,", c->i);
  }
  printf("\n");
}

TEST(ArrayTest, test_remove) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> b(buffer);

  for (int i = 0; i < len; ++i) {
    auto *res = insert(b, ArrayTestData(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }
  ASSERT_FALSE(insert(b, 11));

  int cnt = 0;
  while (b.length > 0) {
    ASSERT_TRUE(remove(b, 0));
    ++cnt;
  }
  ASSERT_EQ(cnt, len);
}

TEST(ArrayTest, test_take) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> b(buffer);

  for (int i = 0; i < len; ++i) {
    auto *res = insert(b, ArrayTestData(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }
  ASSERT_FALSE(insert(b, 11));

  int cnt = 0;
  ArrayTestData cmp(0);
  while (b.length > 0) {
    ArrayTestData out(9999);
    ASSERT_TRUE(take(b, std::size_t(0), out));
    // printf("take: %d\n", out.i);
    ASSERT_EQ(cmp.i, out.i);
    cmp.i = b.length;
    ++cnt;
  }
  ASSERT_EQ(cnt, len);
}
