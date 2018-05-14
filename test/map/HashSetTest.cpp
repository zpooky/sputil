#include <gtest/gtest.h>
#include <hash/fnv.h>
#include <list/SkipList.h>
#include <map/Set.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <util/assert.h>

static std::size_t
identity(const std::size_t &in) {
  return in;
}

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
      printf(" insert(set, i[%zu])\n", i);
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

  TestHashSetTest(const TestHashSetTest &) = delete;
  TestHashSetTest(TestHashSetTest &&o)
      : TestHashSetTest(o.data) {
  }

  TestHashSetTest &
  operator=(const TestHashSetTest &&) = delete;
  TestHashSetTest &
  operator=(const TestHashSetTest &) = delete;

  bool
  operator==(std::size_t o) const noexcept {
    return data == o;
  }

  bool
  operator==(const TestHashSetTest &o) const noexcept {
    return operator==(o.data);
  }

  bool
  operator>(const std::size_t &o) const noexcept {
    return data > o;
  }

  bool
  operator>(const TestHashSetTest &o) const noexcept {
    return operator>(o.data);
  }

  ~TestHashSetTest() {
    assertxs(active > 0, active);
    --active;
    assertxs(active >= 0, active);
  }
};

std::int64_t TestHashSetTest::active = 0;

static std::size_t
fnv_hash(const TestHashSetTest &in) noexcept {
  return fnv_1a::encode32(&in.data, sizeof(in));
}

TEST(HashSetTest, test_dtor) {
  {
    sp::HashSet<TestHashSetTest, fnv_hash> set;

    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        {
          auto *res = insert(set, a);
          ASSERT_FALSE(res);
        }
        {
          auto *res = lookup(set, TestHashSetTest(a));
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, a);
        }
      }
      {
        auto *res = insert(set, i);

        ASSERT_TRUE(res);
        ASSERT_EQ(*res, i);
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), TestHashSetTest::active);
}

TEST(HashSetTest, test_rehash) {
  {
    prng::xorshift32 r(1);
    sp::SkipList<std::size_t, 8> inserted;
    sp::HashSet<TestHashSetTest, fnv_hash> set;

    for (std::size_t i = 0; i < 1024 * 128; ++i) {
      // for_each(inserted, [&set](auto &a) {
      //   auto *res = insert(set, a);
      //   ASSERT_FALSE(res);
      //
      //   {
      //     auto *res = lookup(set, a);
      //     ASSERT_TRUE(res);
      //     ASSERT_EQ(*res, a);
      //   }
      // });
      // ASSERT_EQ(i, sp::n::length(inserted));

      {
        auto *res = insert(set, i);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, i);
      }
      // {
      //   auto res = insert_unique(inserted, i);
      //   bool inserted = std::get<1>(res);
      //   ASSERT_TRUE(inserted);
      //   auto val = std::get<0>(res);
      //   ASSERT_EQ(*val, i);
      // }
    }
  }
  ASSERT_EQ(std::int64_t(0), TestHashSetTest::active);
}
