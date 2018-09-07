#include <gtest/gtest.h>
#include <hash/fnv.h>
#include <hash/standard.h>
#include <list/SkipList.h>
#include <map/HashSet.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <unordered_set>
#include <util/Bitset.h>
#include <util/assert.h>

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
  operator==(std::size_t o) const noexcept {
    return data == o;
  }

  bool
  operator==(const StrictHashSetTest &o) const noexcept {
    return this->operator==(o.data);
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
  operator()(std::size_t in) const {
    sp::Hasher<std::size_t> h;
    return h(in);
  }
  std::size_t
  operator()(const StrictHashSetTest &in) const {
    return operator()(in.data);
  }
};

std::int64_t StrictHashSetTest::active = 0;

TEST(HashSetTest, test_insert_dtor) {
  using TType = StrictHashSetTest;

  {
    sp::HashSet<TType, vanilla_hash_shst> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        {
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
        {
          const TType *res = lookup(set, a);
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
      }

      ASSERT_EQ(i, sp::rec::length(set));
      {
        ASSERT_FALSE(lookup(set, i));
        ASSERT_FALSE(lookup(set, TType(i, 0)));

        TType *res = insert(set, TType(i, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, i);
        ASSERT_TRUE(sp::rec::verify(set));
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), StrictHashSetTest::active);
}

TEST(HashSetTest, test_upsert_dtor) {
  using TType = StrictHashSetTest;

  {
    sp::HashSet<TType, vanilla_hash_shst> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        {
          TType *res = insert(set, TType(a, 0));
          ASSERT_FALSE(res);
        }
        {
          TType *res = upsert(set, TType(a, 0));
          ASSERT_TRUE(res);
        }
        {
          const TType *res = lookup(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          const TType *res = lookup(set, a);
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
      }

      ASSERT_EQ(i, sp::rec::length(set));
      {
        ASSERT_FALSE(lookup(set, i));
        ASSERT_FALSE(lookup(set, TType(i, 0)));

        TType *res = upsert(set, TType(i, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, i);
        ASSERT_TRUE(sp::rec::verify(set));
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), StrictHashSetTest::active);
}

TEST(HashSetTest, test_lookup_insert_dtor) {
  using TType = StrictHashSetTest;

  {
    sp::HashSet<TType, vanilla_hash_shst> set;
    for (std::size_t i = 0; i < 1024; ++i) {
      for (std::size_t a = 0; a < i; ++a) {
        {
          TType *res = insert(set, TType(a, 0));
          ASSERT_FALSE(res);
        }
        {
          TType *res = upsert(set, TType(a, 0));
          ASSERT_TRUE(res);
        }
        {
          TType *res = lookup_insert(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          const TType *res = lookup(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          const TType *res = lookup(set, a);
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
      }

      ASSERT_EQ(i, sp::rec::length(set));
      {
        ASSERT_FALSE(lookup(set, i));
        ASSERT_FALSE(lookup(set, TType(i, 0)));

        TType *res = lookup_insert(set, TType(i, 0));
        ASSERT_TRUE(res);
        ASSERT_EQ(res->data, i);
        ASSERT_TRUE(sp::rec::verify(set));
      }
    }
  }
  ASSERT_EQ(std::int64_t(0), StrictHashSetTest::active);
}

TEST(HashSetTest, test_lookup_compute_dtor) {
  using TType = StrictHashSetTest;

  auto not_comp = [](auto &, const TType &) {
    /**/
    assertx(false);
  };

  {
    sp::HashSet<TType, vanilla_hash_shst> set;
    for (std::size_t i = 0; i < 1024; ++i) {

      auto comp = [&i](auto &current, const TType &xarx) {
        assertxs(xarx.data == i, xarx.data, i);
        new (&current.value) TType(xarx.data, 0);
      };

      for (std::size_t a = 0; a < i; ++a) {
        {
          TType *res = insert(set, TType(a, 0));
          ASSERT_FALSE(res);
        }
        {
          TType *res = lookup_compute(set, TType(a, 0), not_comp);
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          TType *res = upsert(set, TType(a, 0));
          ASSERT_TRUE(res);
        }
        {
          TType *res = lookup_insert(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          const TType *res = lookup(set, TType(a, 0));
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
        {
          const TType *res = lookup(set, a);
          ASSERT_TRUE(res);
          ASSERT_EQ(res->data, a);
        }
      }

      ASSERT_EQ(i, sp::rec::length(set));
      {
        ASSERT_FALSE(lookup(set, i));
        ASSERT_FALSE(lookup(set, TType(i, 0)));

        TType *res = lookup_compute(set, TType(i, 0), comp);
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
  sp::HashSet<std::uint32_t> set;
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

struct THSTHasher {
  std::size_t
  operator()(const TestHashSetTest &in) noexcept {
    return fnv_1a::encode32(&in.data, sizeof(in.data));
  }
};

TEST(HashSetTest, test_dtor) {
  {
    sp::HashSet<TestHashSetTest, THSTHasher> set;

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
  // for (; seed < 10000000; ++seed) //
  {
    printf("seed[%zu]\n", seed);

    std::size_t inserted = 0;
    std::size_t i = 0;

    prng::xorshift32 r(20000 + seed);
    sp::HashSet<std::uint32_t> set_insert;
    sp::HashSet<std::uint32_t> set_upsert;
    sp::HashSet<std::uint32_t> set_look_insert;
    sp::HashSet<std::uint32_t> set_compute;
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
      std::uint32_t *res = insert(set_insert, in);

      bool comp = false;
      ASSERT_TRUE(lookup_compute(
          set_compute, in, [in, &comp](auto &current, const auto &xarx) {
            comp = true;
            assertxs(in == xarx, in, xarx);
            return new (&current.value) std::uint32_t(xarx);
          }));

      if (res) {
        ASSERT_EQ(*res, in);
        ++inserted;
        ASSERT_FALSE(test(b, in));
        ASSERT_FALSE(sp::set(b, in, true));
        ASSERT_TRUE(comp);
      } else {
        // already inserted
        ASSERT_TRUE(test(b, in));
        ASSERT_FALSE(comp);
      }
      ASSERT_TRUE(upsert(set_upsert, in));
      ASSERT_TRUE(lookup_insert(set_look_insert, in));
    }

    ASSERT_EQ(inserted, sp::rec::length(set_insert));
    ASSERT_EQ(inserted, sp::rec::length(set_upsert));
    ASSERT_EQ(inserted, sp::rec::length(set_look_insert));
    ASSERT_EQ(inserted, sp::rec::length(set_compute));
    printf("inserted:%zu\n", inserted);

    std::size_t fins = 0;
    for_each(b, [&](std::size_t idx, bool present) {
      ASSERT_EQ(test(b, idx), present);

      std::uint32_t *l_ins = lookup(set_insert, idx);
      std::uint32_t *l_ups = lookup(set_upsert, idx);
      std::uint32_t *l_look_ins = lookup(set_look_insert, idx);
      std::uint32_t *l_comp = lookup(set_look_insert, idx);
      if (present) {
        ++fins;
        ASSERT_TRUE(l_ins);
        ASSERT_EQ(idx, *l_ins);

        ASSERT_TRUE(l_ups);
        ASSERT_EQ(idx, *l_ups);

        ASSERT_TRUE(l_look_ins);
        ASSERT_EQ(idx, *l_look_ins);

        ASSERT_TRUE(l_comp);
        ASSERT_EQ(idx, *l_comp);
      } else {
        ASSERT_FALSE(l_ins);
        ASSERT_FALSE(l_ups);
        ASSERT_FALSE(l_look_ins);
        ASSERT_FALSE(l_comp);
      }
    });

    ASSERT_EQ(inserted, fins);
    printf("done\n");
  }
}

TEST(HashSetTest, test_lookup_insert) {
  sp::HashSet<std::uint32_t> set;
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

TEST(HashSetTest, test_lookup_compute) {
  sp::HashSet<std::uint32_t> set;
  ASSERT_EQ(0, sp::rec::length(set));
  bool first_cb = false;
  std::uint32_t *first = lookup_compute(
      set, 1, [&first_cb](auto &current, const std::uint32_t &value) {
        first_cb = true;
        return new (&current.value) std::uint32_t(value);
      });

  ASSERT_TRUE(first_cb);
  ASSERT_EQ(1, sp::rec::length(set));
  ASSERT_EQ(*first, 1);

  bool second_cb = false;
  std::uint32_t *second =
      lookup_compute(set, 1, [&second_cb](auto &, const std::uint32_t &) {
        second_cb = true;
        return nullptr;
      });
  ASSERT_FALSE(second_cb);
  ASSERT_EQ(*first, 1);
  ASSERT_EQ(*first, *second);
  ASSERT_EQ(first, second);
  ASSERT_EQ(1, sp::rec::length(set));
}

#if 0

TEST(HashSetTest, test_lookup_compute2) {
  // sp::LinkedList<int> nodes;
  std::size_t insxx = 0;

  auto factory = [&insxx](auto &state, int id) {
    insxx++;
    // printf("  2.push_back(%d)\n", id);
    // int *res = push_back(nodes, id);
    // assertx(res);

    new (&state.value) int(id);
  };

  sp::HashSet<int> set;

  int *zero = lookup_compute(set, 0, factory);
  ASSERT_TRUE(zero);
  ASSERT_EQ(*zero, 0);
  ASSERT_EQ(1, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *one = lookup_compute(set, 1, factory);
  ASSERT_TRUE(one);
  ASSERT_EQ(*one, 1);
  ASSERT_EQ(2, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *three = lookup_compute(set, 3, factory);
  ASSERT_TRUE(three);
  ASSERT_EQ(*three, 3);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *three_x = lookup_compute(set, 3, factory);
  ASSERT_TRUE(three_x);
  ASSERT_EQ(*three_x, 3);
  ASSERT_EQ(three, three_x);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  int *one_x = lookup_compute(set, 1, factory);
  ASSERT_TRUE(one_x);
  ASSERT_EQ(*one_x, 1);
  ASSERT_EQ(one_x, one);
  ASSERT_EQ(3, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  printf("--\n");
  int *minus_one = lookup_compute(set, -1, factory);
  printf("--\n");
  ASSERT_TRUE(minus_one);
  ASSERT_EQ(*minus_one, -1);
  ASSERT_EQ(4, insxx);
  ASSERT_EQ(insxx, sp::rec::length(set));

  ASSERT_EQ(*zero, 0);
  ASSERT_EQ(*one, 1);
  ASSERT_EQ(*three, 3);
  ASSERT_EQ(three_x, three);
  ASSERT_EQ(one_x, one);
  ASSERT_EQ(*minus_one, -1);

  {
    int *l_zero = lookup(set, 0);
    ASSERT_EQ(l_zero, zero);
    int *l_one = lookup(set, 1);
    ASSERT_EQ(l_one, one);
    int *l_three = lookup(set, 3);
    ASSERT_EQ(l_three, three);
    int *l_minus_one = lookup(set, -1);
    ASSERT_EQ(l_minus_one, minus_one);
  }
}

#endif

struct HSTDummy {
  const int id;
  explicit HSTDummy(int i)
      : id(i) {
  }

  ~HSTDummy() {
  }
};

struct HSTHasher {
  std::size_t
  operator()(int id) const {
    return id;
  }

  std::size_t
  operator()(const HSTDummy *in) const {
    assertx(in);
    return this->operator()(in->id);
  }
};

struct HSTDummyEq {
  bool
  operator()(const HSTDummy *f, int id) const noexcept {
    return f->id == id;
  }

  bool
  operator()(const HSTDummy *f, const HSTDummy *s) const noexcept {
    return operator()(f, s->id);
  }
};

TEST(HashSetTest, test_lookup_compute3) {
  HSTDummy one_storage{1};

  std::size_t insxx = 0;
  auto factory2 = [&insxx, &one_storage](auto &state, int id) {
    printf("--\n");
    assertx(id == 1);
    insxx++;
    new (&state.value) HSTDummy *(&one_storage);

    HSTHasher hash;
    std::size_t stor_hash = hash(&one_storage);
    std::size_t id_hash = hash(id);
    std::size_t st_hash = hash(*((HSTDummy **)&state.value));

    assertxs(stor_hash == id_hash, stor_hash, id_hash);
    assertxs(st_hash == id_hash, stor_hash, st_hash, id_hash);
  };

  auto factory = [factory2](auto &state, HSTDummy *id) {
    assertx(id);

    factory2(state, id->id);
  };

  sp::HashSet<HSTDummy *, HSTHasher, HSTDummyEq> set;
  for (int i = 0; i < 100; ++i) {
    HSTDummy one_in{1};
    HSTDummy **one = lookup_compute(set, &one_in, factory);
    ASSERT_TRUE(one);
    ASSERT_TRUE(*one);
    ASSERT_EQ((*one)->id, 1);
    ASSERT_EQ(1, insxx);
    ASSERT_EQ(1, sp::rec::length(set));

    {
      HSTDummy **one_x = lookup_compute(set, &one_in, factory);
      ASSERT_TRUE(one_x);
      ASSERT_TRUE(*one_x);
      ASSERT_EQ(one, one_x);
      ASSERT_EQ(*one, *one_x);
      ASSERT_EQ((*one_x)->id, 1);
      ASSERT_EQ(1, insxx);
      ASSERT_EQ(1, sp::rec::length(set));
    }

    {
      HSTDummy **l_one = lookup(set, 1);
      ASSERT_TRUE(l_one);
      ASSERT_TRUE(*l_one);
      ASSERT_EQ((*l_one)->id, 1);
      ASSERT_EQ(one, l_one);
      ASSERT_EQ(*one, *l_one);
      ASSERT_EQ(1, sp::rec::length(set));
    }

    {
      HSTDummy **one_x = lookup_compute(set, 1, factory2);
      ASSERT_TRUE(one_x);
      ASSERT_TRUE(*one_x);
      ASSERT_EQ(one, one_x);
      ASSERT_EQ(*one, *one_x);
      ASSERT_EQ((*one_x)->id, 1);
      ASSERT_EQ(1, insxx);
      ASSERT_EQ(1, sp::rec::length(set));
    }
  }
}

TEST(HashSetTest, hasherxx) {
  {
    sp::Hasher<signed char> h;
    std::size_t hash = h('c');
    printf("signed char: %zu\n", hash);
  }
  {
    sp::Hasher<unsigned char> h;
    std::size_t hash = h('c');
    printf("unsigned char: %zu\n", hash);
  }
}
