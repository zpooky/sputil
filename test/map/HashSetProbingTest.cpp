// #define NDEBUG

#include <collection/Array.h>
#include <gtest/gtest.h>
#include <map/HashSet.h>
#include <map/HashSetProbing.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>
#include <util/Timer.h>

namespace sp {
struct HSPTDummy {
  static std::int64_t active;
  const int key;
  long alignment0;
  long alignment1;
  long alignment2;

  explicit HSPTDummy(int k)
      : key(k)
      , alignment0(active)
      , alignment1(active)
      , alignment2(active) {
    // printf("ctor[%u]\n", active);
    ++active;
  }

  HSPTDummy(HSPTDummy &&o)
      : HSPTDummy(o.key) {
  }

  HSPTDummy(const HSPTDummy &) = delete;

  HSPTDummy &
  operator=(const HSPTDummy &) = delete;
  HSPTDummy &
  operator=(const HSPTDummy &&) = delete;

  bool
  operator==(int o) const noexcept {
    return key == o;
  }
  bool
  operator==(const HSPTDummy &o) const noexcept {
    return operator==(o.key);
  }

  ~HSPTDummy() {
    // printf("dtor %p, active[%u]\n", this, active);
    // printf("dtor[%u]\n", active);
    assertxs(active > 0, active);
    --active;
    assertxs(active >= 0, active);
  }
};

template <>
struct Hasher<HSPTDummy> {
  std::size_t
  operator()(int o) const noexcept {
    Hasher<int> h;
    return h(o);
  }

  std::size_t
  operator()(const HSPTDummy &o) const noexcept {
    return operator()(o.key);
  }
};

std::int64_t HSPTDummy::active = 0;

static void
swap(HSPTDummy &f, HSPTDummy &s) noexcept {
  HSPTDummy tmp(std::move(f));
  f.~HSPTDummy();
  new (&f) HSPTDummy(std::move(s));
  s.~HSPTDummy();
  new (&s) HSPTDummy(std::move(tmp));
}
}

template <typename SET>
static void
run_bench(SET &set) noexcept {
  using TT = typename SET::value_type;
  // constexpr int range = (1024 * 10) + 1;
  constexpr int range = (1024 * 4) + 1;
  // constexpr int range = 350;
  // constexpr int range = (16 * 1) + 1;
  // constexpr int range = (1024 * 1) + 1;
  prng::xorshift32 r(1);
  sp::TimerContex ctx;
  // printf("range(%zu)\n", range);
  for (std::size_t a = 0; a < 10; ++a) //
  {
    sp::timer(ctx, [&]() {
      sp::UinDynamicArray<TT> ref(range);

      for (int i = 0; i < range; ++i) {
        const int cmp(i);
        ASSERT_FALSE(lookup(set, i));
        ASSERT_EQ(cmp, i);

        // printf("insert(%d){", i);
        // ASSERT_EQ(i * 2, TT::active);
        TT *const maybe = insert(set, i);
        ASSERT_EQ(cmp, i);

        // ASSERT_EQ(((i * 2) + 1), TT::active);
        // printf("}\n");

        TT *const l = lookup(set, i);
        ASSERT_EQ(cmp, i);

        if (maybe) {
          {
            ASSERT_TRUE(push(ref, i));
            // ASSERT_EQ(((i * 2) + 2), TT::active);
          }
          ASSERT_EQ(cmp, i);
          // printf("%d\n", *maybe);

          ASSERT_TRUE(l);
          ASSERT_EQ(*maybe, *l);
          ASSERT_EQ(*maybe, i);
          ASSERT_EQ(maybe, l);
        } else {
          ASSERT_FALSE(l);
        }

        sp::rec::verify(set);

        for (int in = 0; in < i; ++in) {
          const TT cmp(in);

          TT *l = lookup(set, in);
          ASSERT_EQ(cmp, in);

          if (!l) {
            // printf("length(%zu)\n", length(ref));
            printf("lookup(%d): null\n", in);
          }

          ASSERT_TRUE(l);
          ASSERT_EQ(*l, in);
        }
#if 0
#endif
      } // for

      // #if 0
      // ASSERT_EQ(length(ref), sp::n::length(set));

      sp::rec::verify(set);
      shuffle(r, ref);
      // #endif

      for_each(ref, [&set](TT &in) {
        // const TT cmp(in);
        TT *const l = lookup(set, in);
        // ASSERT_EQ(cmp, in);

        if (!l) {
          // printf("lookup(%d): null\n", in);
        }
        ASSERT_TRUE(l);
        ASSERT_EQ(*l, in);

        ASSERT_TRUE(remove(set, in));
        ASSERT_FALSE(lookup(set, in));
      });

      sp::rec::verify(set);

      for (int i = 0; i < range; ++i) {
        ASSERT_FALSE(lookup(set, i));
        ASSERT_FALSE(remove(set, i));
      }

      sp::rec::verify(set);

      // ASSERT_EQ(std::size_t(0), sp::n::length(set));
      // #if 0
      // #endif

      // printf("----\n");
      // avl::dump(set.tree);
    });
  }

  print(ctx);

  printf("Average: ");
  print(average(ctx));

  printf("Median: ");
  print(median(ctx));

  // printf("length:%zu\n", length);
}

// #if 0
TEST(HashSetProbingTest, test_HashSetProbing) {
  sp::HashSetProbing<int> set;
  run_bench(set);
  std::size_t cnt = 0;
  for (std::size_t i = 0; i < set.capacity; ++i) {
    if (sp::test(set.tags, i) != HSPTag_EMPTY) {
      ++cnt;
      // ASSERT_EQ(set.table[i].tag, HSPTag_EMPTY);
    }
  }
  ASSERT_EQ(std::size_t(0), cnt);
}
// #endif

TEST(HashSetProbingTest, test_HashSetProbing_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::HSPTDummy::active);
  {
    sp::HashSetProbing<sp::HSPTDummy> set;
    run_bench(set);
    std::size_t cnt = 0;
    for (std::size_t i = 0; i < set.capacity; ++i) {
      if (sp::test(set.tags, i) != HSPTag_EMPTY) {
        ++cnt;
        // ASSERT_EQ(set.table[i].tag, HSPTag_EMPTY);
      }
    }
    ASSERT_EQ(std::size_t(0), cnt);
    printf("length: %zu\n", sp::n::length(set));
  }
  ASSERT_EQ(std::int64_t(0), sp::HSPTDummy::active);
}

// #if 0
TEST(HashSetProbingTest, test_HashSetTree) {
  sp::HashSet<int> set;
  run_bench(set);
}
// #endif

static void
test_dump(sp::HashSetProbing<std::uint32_t> &set) {
#if 0
  if (set.table) {
    printf("|");
    for (std::size_t i = 0; i < capacity(set); ++i) {
      auto &bucket = set.table[i];
      if (bucket.tag == sp::impl::HSPTag::PRESENT) {
        std::uint32_t *const current = (std::uint32_t *)&bucket.value;
        printf("%02u|", *current);
      } else if (bucket.tag == sp::impl::HSPTag::EMPTY) {
        printf("--|");
      } else {
        printf("xx|");
      }
    }
    printf("\n");
  }
#endif
  (void)set;
}

// #if 0

TEST(HashSetProbingTest, test_rand) {
  prng::xorshift32 r(1459312133);

  constexpr std::size_t range = (1024 * 4) + 1;
  constexpr std::size_t bits(sizeof(std::uint64_t) * 8);
  // ASSERT_TRUE(range % bits == 0);

  // const std::uint32_t r_max = range * 9;
  const std::uint32_t r_max = range;

  const std::size_t length = sp::bitset_number_of_buffer(range);
  auto raw = new uint64_t[length];
  ASSERT_TRUE(raw);

  do {
    std::size_t inserted = 0;
    printf("seed[%u]\n", r.state);
    memset(raw, 0, length * sizeof(uint64_t));
    for (std::size_t i = 0; i < length; ++i) {
      ASSERT_EQ(raw[i], 0);
    }
    sp::HashSetProbing<std::uint32_t> set;
    sp::Bitset present(raw, length);
    // sp::HashSet<std::uint32_t> present2;

    for (std::size_t i = 0; i < range; ++i) {
      std::size_t found = 0;
      for_each(present, [&set, &found](std::size_t idx, bool v) {
        const std::uint32_t in(idx);
        auto res = lookup(set, in);
        if (v) {
          ASSERT_TRUE(res);
          ASSERT_EQ(*res, in);
          ++found;
        } else {
          ASSERT_FALSE(res);
        }
      });

      ASSERT_FALSE(lookup(set, std::uint32_t(~int(0))));

      ASSERT_EQ(inserted, found);

      const std::uint32_t current = uniform_dist(r, 0, r_max);

      const std::uint32_t *const l_before = lookup(set, current);
      const std::uint32_t *const res = insert(set, current);
      const std::uint32_t *const l_after = lookup(set, current);

      ASSERT_FALSE(insert(set, current));

      const bool already = test(present, std::size_t(current));
      if (already) {
        // printf("rem[%u]\n", current);
        if (res) {
          printf("bad[%u]\n", current);
        }

        ASSERT_TRUE(l_before);
        ASSERT_TRUE(l_after);

        ASSERT_EQ(*l_before, current);
        ASSERT_EQ(*l_after, current);
        ASSERT_EQ(l_before, l_after);

        ASSERT_FALSE(res);

        {
          ASSERT_TRUE(remove(set, current));
          test_dump(set);
          ASSERT_FALSE(lookup(set, current));

          --inserted;
          ASSERT_TRUE(sp::set(present, std::size_t(current), false));
          ASSERT_FALSE(sp::test(present, std::size_t(current)));
          ASSERT_FALSE(sp::set(present, std::size_t(current), false));
        }
      } else {
        // printf("ins[%u]\n", current);
        test_dump(set);

        ++inserted;
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, current);

        ASSERT_FALSE(l_before);
        ASSERT_TRUE(l_after);
        ASSERT_EQ(l_after, res);

        ASSERT_FALSE(sp::test(present, std::size_t(current)));
        ASSERT_FALSE(sp::set(present, std::size_t(current), true));
        ASSERT_TRUE(sp::test(present, std::size_t(current)));
        ASSERT_TRUE(sp::set(present, std::size_t(current), true));
      }
    } // for

    for_each(present, [&set](std::size_t idx, bool v) {
      const std::uint32_t in(idx);
      auto res = lookup(set, in);
      if (v) {
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, in);
        ASSERT_TRUE(remove(set, in));
      } else {
        ASSERT_FALSE(res);
        ASSERT_FALSE(remove(set, in));
      }
      ASSERT_FALSE(lookup(set, in));
    });

    {
      std::size_t cnt = 0;
      for (std::size_t i = 0; i < set.capacity; ++i) {
        if (sp::test(set.tags, i) != HSPTag_EMPTY) {
          ++cnt;
          // ASSERT_EQ(set.table[i].tag, HSPTag_EMPTY);
        }
      }
      ASSERT_EQ(std::size_t(0), cnt);
    }

  } while (true);

  delete[] raw;
}

// #endif
