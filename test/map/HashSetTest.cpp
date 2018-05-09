#include <gtest/gtest.h>
#include <hash/fnv.h>
#include <map/Set.h>
#include <util/assert.h>
// #include <prng/util.h>
// #include <prng/xorshift.h>

static std::size_t
identity(const std::size_t &in) {
  return in;
}

TEST(HashSetTest, test_xx) {
  sp::HashSet<std::size_t, identity> set;
  {
    std::size_t *res = insert(set, 0);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 0);
  }
  {
    std::size_t *res = insert(set, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    std::size_t *res = insert(set, 256);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 256);
  }
}

// template <typename T, typename h>
// static void
// generic_insert_lookup(sp::HashSet<std::size_t, identity> &set,
//                       std::size_t its) {
// }

TEST(HashSetTest, test) {
  sp::HashSet<std::size_t, identity> set;
  std::size_t i = 0;
  for (; i < 512; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        std::size_t *res = insert(set, a);
        ASSERT_FALSE(res);
      }
      {
        const std::size_t *res = lookup(set, a);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, a);
      }
    }
    {
      std::size_t *res = insert(set, i);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
  }
}

struct TestHashSetTest {
  static std::int64_t active;
  const std::size_t data;

  TestHashSetTest(std::size_t d)
      : data(d) {
    assertxs(active >= 0, active);
    ++active;
  }

  bool
  operator==(std::size_t o) const noexcept {
    return data == o;
  }

  bool
  operator==(const TestHashSetTest &o) const noexcept {
    return operator==(o.data);
  }

  ~TestHashSetTest() {
    assertxs(active > 0, active);
    --active;
    assertxs(active >= 0, active);
  }
};

std::int64_t TestHashSetTest::active = 0;

static std::size_t
tst_hash(const TestHashSetTest &in) noexcept {
  return fnv_1a::encode32(&in.data, sizeof(in));
}

TEST(HashSetTest, test_dtor) {
  sp::HashSet<TestHashSetTest, tst_hash> set;

  for (std::size_t i = 0; i < 512; ++i) {
    for (std::size_t a = 0; a < i; ++a) {
      {
        auto *res = insert(set, TestHashSetTest(a));
        ASSERT_FALSE(res);
      }
      {
        auto *res = lookup(set, TestHashSetTest(a));
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, a);
      }
    }
    {
      auto *res = insert(set, TestHashSetTest(i));

      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
  }
  ASSERT_EQ(std::int64_t(0), TestHashSetTest::active);
}
