#include <gtest/gtest.h>
#include <hash/fnv.h>
#include <list/SkipList.h>
#include <map/HashSet.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <unordered_set>
#include <util/Bitset.h>
#include <util/assert.h>

// static std::size_t
// identity(const std::size_t &in) {
//   return in;
// }

static std::size_t
identity(const std::uint32_t &in) {
  std::hash<std::uint32_t> h;
  return h(in);
  // return std::size_t(in);
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

struct vanilla_hash_shst {
  std::size_t
  operator()(const StrictHashSetTest &in) const {
    return identity(in.data);
  }
};

std::int64_t StrictHashSetTest::active = 0;

static std::size_t
identity(const StrictHashSetTest &in) {
  return std::size_t(in.data);
}

TEST(HashSetTest, test) {
  // using TType = std::size_t;
  using TType = StrictHashSetTest;

  {
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

TEST(HashSetTest, test_vanilla) {
  using TType = StrictHashSetTest;
  // using TType = std::uint32_t;

  {
    std::unordered_set<TType, vanilla_hash_shst> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        auto res = set.insert(TType(a, 0));
        ASSERT_FALSE(res.second);
        {
          TType needle(a, 0);
          auto res = set.find(needle);
          ASSERT_TRUE(res != set.end());
          ASSERT_EQ(res->data, a);
        }
      }

      {
        auto res = set.insert(TType(i, 0));
        ASSERT_TRUE(res.second);
        ASSERT_EQ(res.first->data, i);
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
        bool priv = sp::set(bset, std::size_t(current), true);
        ASSERT_EQ(false, priv);
      }
    }
  } // for
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
  return fnv_1a::encode32(&in.data, sizeof(in.data));
}

static std::size_t
fnv_hash(const std::uint32_t &in) noexcept {
  return fnv_1a::encode32(&in, sizeof(in));
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

TEST(HashSetTest, test_afew) {
  std::size_t seed = 1;
  for (; seed < 10000000; ++seed) //
  {
    printf("seed[%zu]\n", seed);

    std::size_t inserted = 0;
    std::size_t i = 0;

    prng::xorshift32 r(10000 + seed);
    sp::HashSet<std::uint32_t, fnv_hash> set;
    sp::HashSet<std::uint32_t, fnv_hash> set2;
    constexpr std::size_t limit_i = 3100000;
    constexpr std::size_t max_dist = 3233123;

    sp::DynamicBitset b((max_dist / 64) + 64);
    printf("bits(): %zu\n", bits(b));
    for (std::size_t i = 0; i < bits(b); ++i) {
      ASSERT_FALSE(sp::set(b, i, true));
      ASSERT_TRUE(sp::set(b, i, false));
    }

    ASSERT_FALSE(sp::set(b, max_dist, true));
    ASSERT_TRUE(sp::set(b, max_dist, false));

    while (i++ < limit_i) {
      if (i % 10000 == 0) {
        printf(".%zu\n", i);
      }
      const std::size_t in = uniform_dist(r, 0, max_dist);
      std::uint32_t *res = insert(set, in);
      if (res) {
        ASSERT_EQ(*res, in);
        ++inserted;
        ASSERT_FALSE(test(b, in));
        ASSERT_FALSE(sp::set(b, in, true));
      } else {
        ASSERT_TRUE(test(b, in));
      }
      ASSERT_TRUE(upsert(set2, in));
    }

    ASSERT_EQ(inserted, sp::rec::length(set));
    printf("inserted:%zu\n", inserted);

    std::size_t fins = 0;
    for_each(b, [&](std::size_t idx, bool present) {
      ASSERT_EQ(test(b, idx), present);

      std::uint32_t *l = lookup(set, idx);
      std::uint32_t *l2 = lookup(set2, idx);
      if (present) {
        ++fins;
        ASSERT_TRUE(l);
        ASSERT_EQ(idx, *l);

        ASSERT_TRUE(l2);
        ASSERT_EQ(idx, *l2);
      } else {
        ASSERT_FALSE(l);
        ASSERT_FALSE(l2);
      }
    });

    ASSERT_EQ(inserted, fins);
    printf("done\n");
  }
}

TEST(HashSetTest, test_lookup_insert) {
  sp::HashSet<std::uint32_t, fnv_hash> set;
  ASSERT_EQ(0, sp::rec::length(set));
  std::uint32_t *first = lookup_insert(set, 1);
  ASSERT_EQ(1, sp::rec::length(set));
  ASSERT_EQ(*first, 1);
  std::uint32_t *second = lookup_insert(set, 1);
  ASSERT_EQ(*first, 1);
  ASSERT_EQ(*first, *second);
  ASSERT_EQ(first, second);
  ASSERT_EQ(1, sp::rec::length(set));
}
