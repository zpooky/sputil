#include <gtest/gtest.h>
#include <hash/fnv.h>
#include <list/SkipList.h>
#include <map/HashSet.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>
#include <util/assert.h>

// static std::size_t
// identity(const std::size_t &in) {
//   return in;
// }

static std::size_t
identity(const std::uint32_t &in) {
  return std::size_t(in);
}

struct StrictHashSetTest {
  static std::int64_t active;
  const std::size_t data;

  explicit StrictHashSetTest(std::size_t d, int)
      : data(d) {
    // printf("\nctor %p\n", this);
    ++active;
  }

  StrictHashSetTest(const StrictHashSetTest &) = delete;
  StrictHashSetTest(StrictHashSetTest &&o)
      : data(o.data) {
    // printf("\nmove ctor %p <- %p\n", this, &o);
    ++active;
  }

  StrictHashSetTest &
  operator=(const StrictHashSetTest &&) = delete;
  StrictHashSetTest &
  operator=(const StrictHashSetTest &) = delete;

  bool
  operator==(const StrictHashSetTest &o) const noexcept {
    return data == o.data;
  }

  bool
  operator>(const StrictHashSetTest &o) const noexcept {
    return data > o.data;
  }

  explicit operator std::size_t() const noexcept {
    return data;
  }

  ~StrictHashSetTest() {
    // printf("dtor %p\n", this);
    assertxs(active > 0, active);
    --active;
    assertxs(active >= 0, active);
  }
};

std::int64_t StrictHashSetTest::active = 0;

static std::size_t
identity(const StrictHashSetTest &in) {
  return std::size_t(in.data);
}

TEST(HashSetTest, test) {
  {
    // using TType = std::size_t;
    using TType = StrictHashSetTest;

    sp::HashSet<TType, identity> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        {
          // if (i == 311 && a == 256) {
          //   printf("");
          // }
          TType *res = insert(set, TType(a, 0));
          if (res) {
            printf("i[%zu] a[%zu] = res[%zu]\n", i, a, std::size_t(*res));
            dump(set.tree);
          }
          ASSERT_FALSE(res);
        }
        {
          const TType *res = lookup(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
      }

      ASSERT_EQ(i, sp::rec::length(set));
      {
        TType *res = insert(set, TType(i, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, i);
        ASSERT_TRUE(sp::rec::verify(set));
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), StrictHashSetTest::active);
}

TEST(HashSetTest, test_rand) {
  sp::HashSet<std::uint32_t, identity> set;
  prng::xorshift32 r(1);
  std::size_t i = 0;
  constexpr std::uint32_t range = 1024 * 9;
  constexpr std::size_t bits(sizeof(std::uint64_t) * 8);
  ASSERT_TRUE(range % bits == 0);
  std::size_t length = range / bits;

  auto raw = new uint64_t[length]{0};
  sp::Bitset bset(raw, length);

  for (; i < range; ++i) {
    for_each(bset, [&set](std::size_t idx, bool v) {
      const std::uint32_t in(idx);
      auto res = lookup(set, in);
      if (v) {
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in);
      } else {
        ASSERT_FALSE(res);
      }
    });

    const std::uint32_t current = uniform_dist(r, 0, range);
    {
      const bool v = test(bset, current);
      auto res = insert(set, std::size_t(current));
      if (v) {
        ASSERT_FALSE(res);
      } else {
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, current);
        ASSERT_EQ(false, sp::set(bset, std::size_t(current), true));
      }
    }
  }
  ASSERT_TRUE(sp::rec::verify(set));
  // dump(set.tree);

  delete[] raw;
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
