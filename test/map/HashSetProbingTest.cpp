// #define NDEBUG
#include <test/gcstruct.h>

#include <collection/Array.h>
#include <gtest/gtest.h>
#include <map/HashSetProbing.h>
#include <map/HashSetTree.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>
#include <util/Timer.h>

template <typename SET>
static void
run_bench(prng::xorshift32 &r, sp::TimerContext &ctx, SET &set) noexcept {
  using TT = typename SET::value_type;
  using namespace sp::rec;

  // constexpr int range = (1024 * 10) + 1;
  constexpr int range = (1024 * 4) + 1;
  // constexpr int range = 350;
  // constexpr int range = (16 * 1) + 1;
  // constexpr int range = (1024 * 1) + 1;
  // printf("range(%zu)\n", range);
  sp::UinDynamicArray<TT> ref(range);

  sp::timer(ctx, [&]() {
    for (int i = 0; i < range; ++i) {
      ASSERT_EQ(i, length(set));
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

      ASSERT_EQ(i + 1, length(set));

      sp::rec::verify(set);

      for (int in = 0; in < i; ++in) {
        // const TT cmp(in);

        TT *l = lookup(set, in);
        // ASSERT_EQ(cmp, in);

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

      std::size_t len = length(set);
      ASSERT_TRUE(remove(set, in));
      ASSERT_FALSE(lookup(set, in));
      ASSERT_EQ(len - 1, length(set));
    });

    sp::rec::verify(set);
    ASSERT_EQ(0, length(set));

    for (int i = 0; i < range; ++i) {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_EQ(0, length(set));
      ASSERT_FALSE(remove(set, i));
      ASSERT_EQ(0, length(set));
    }

    sp::rec::verify(set);

    // ASSERT_EQ(std::size_t(0), sp::n::length(set));
    // #if 0
    // #endif

    // printf("----\n");
    // avl::dump(set.tree);
  });

  // printf("length:%zu\n", length);
}

// #if 0
TEST(HashSetProbingTest, test_HashSetProbing) {
  prng::xorshift32 r(1);
  sp::TimerContext ctx;
  for (std::size_t a = 0; a < 10; ++a) {
    sp::HashSetProbing<int> set;

    run_bench(r, ctx, set);
    ASSERT_EQ(std::size_t(0), length(set));
    ASSERT_EQ(std::size_t(0), sp::n::length(set));

    std::size_t cnt = 0;
    for (std::size_t i = 0; i < set.capacity; ++i) {
      if (sp::test(set.tags, i) != HSPTag_EMPTY) {
        ++cnt;
        // ASSERT_EQ(set.table[i].tag, HSPTag_EMPTY);
      }
    }
    ASSERT_EQ(std::size_t(0), cnt);
  }

  print(ctx);

  printf("Average: ");
  print(average(ctx));

  printf("Median: ");
  print(median(ctx));
}
// #endif

TEST(HashSetProbingTest, test_HashSetProbing_dtor) {
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
  {
    prng::xorshift32 r(1);
    sp::TimerContext ctx;
    for (std::size_t a = 0; a < 10; ++a) {
      sp::HashSetProbing<sp::GcStruct> set;

      run_bench(r, ctx, set);
      ASSERT_EQ(std::size_t(0), length(set));
      ASSERT_EQ(std::size_t(0), sp::n::length(set));

      std::size_t cnt = 0;
      for (std::size_t i = 0; i < set.capacity; ++i) {
        if (sp::test(set.tags, i) != HSPTag_EMPTY) {
          ++cnt;
          // ASSERT_EQ(set.table[i].tag, HSPTag_EMPTY);
        }
      }
      ASSERT_EQ(std::size_t(0), cnt);
      // printf("length: %zu\n", sp::n::length(set));
    }

    print(ctx);

    printf("Average: ");
    print(average(ctx));

    printf("Median: ");
    print(median(ctx));
  }
  ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
}

// #if 0
TEST(HashSetProbingTest, test_HashSetTree) {
  prng::xorshift32 r(1);
  sp::TimerContext ctx;
  for (std::size_t a = 0; a < 10; ++a) {
    sp::HashSetTree<int> set;
    run_bench(r, ctx, set);
  }

  print(ctx);

  printf("Average: ");
  print(average(ctx));

  printf("Median: ");
  print(median(ctx));
}
// #endif

TEST(HashSetProbingTest, insert) {
  // HashSetProbingTest.insert (557 ms): 1024 * 256
  sp::TimerContext ctx;
  /* eager: 1024 * 100
   * Average: 171 msec
   * Median: 172 msec
   */
  for (std::size_t i = 0; i < 10; ++i) {
    sp::timer(ctx, []() {
      constexpr int range = (1024 * 10);
      ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
      bool repeat = true;
      {
        sp::HashSetProbing<sp::GcStruct> set;
        {
        Lrepeat:
          for (int i = 0; i < range; ++i) {
            // printf("insert(%d){\n", i);
            sp::GcStruct *const i_res = insert(set, std::size_t(i));
            // printf("}\n");
            ASSERT_TRUE(i_res);
            ASSERT_EQ(i_res->data, i);
          }

          for (int i = 0; i < range; ++i) {
            sp::GcStruct *l_res = lookup(set, i);
            ASSERT_TRUE(l_res);
            ASSERT_EQ(l_res->data, i);
          }

          for (int i = 0; i < range; ++i) {
            ASSERT_TRUE(remove(set, i));
          }

          for (int i = 0; i < range; ++i) {
            sp::GcStruct *l_res = lookup(set, i);
            ASSERT_FALSE(l_res);
          }
          if (repeat) {
            repeat = false;
            goto Lrepeat;
          }
        }
      }
      ASSERT_EQ(std::int64_t(0), sp::GcStruct::active);
    });
  }

  print(ctx);

  printf("Average: ");
  print(average(ctx));

  printf("Median: ");
  print(median(ctx));
}

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
  // prng::xorshift32 r(1459312133);
  // prng::xorshift32 r(1940300157);
  // prng::xorshift32 r(3497049056);
  prng::xorshift32 r(1581117242);

  constexpr std::size_t range = (1024 * 4);
  const std::uint32_t r_max = range;

  const std::size_t length = sp::Bitset_number_of_buffer(range);
  auto raw = new uint64_t[length];
  ASSERT_TRUE(raw);

  do {
    std::size_t inserted = 0;
    printf("seed[%u], length[%zu], range[%zu]\n", r.state, length, range);
    memset(raw, 0, length * sizeof(uint64_t));
    for (std::size_t i = 0; i < length; ++i) {
      ASSERT_EQ(raw[i], 0);
    }
    sp::HashSetProbing<std::uint32_t> set;
    sp::Bitset present(raw, length);
    // sp::HashSetTree<std::uint32_t> present2;

    for (std::size_t i = 0; i < range; ++i) {
      ASSERT_EQ(inserted, sp::length(set));
      ASSERT_EQ(inserted, sp::n::length(set));

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

      // ASSERT_EQ(inserted, found);
      ASSERT_EQ(inserted, sp::length(set));
      ASSERT_EQ(inserted, sp::n::length(set));

      const std::uint32_t current = uniform_dist(r, 0, r_max);

      const std::uint32_t *const l_before = lookup(set, current);
      // printf("insert(%u){\n", current);
      const std::uint32_t *const res = insert(set, current);
      // printf("}\n");
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

        ASSERT_FALSE(l_before);
        ASSERT_TRUE(l_after);
        ASSERT_EQ(l_after, res);

        ASSERT_EQ(*res, current);

        ASSERT_FALSE(sp::test(present, std::size_t(current)));
        ASSERT_FALSE(sp::set(present, std::size_t(current), true));
        ASSERT_TRUE(sp::test(present, std::size_t(current)));
        ASSERT_TRUE(sp::set(present, std::size_t(current), true));
      }

      ASSERT_EQ(inserted, sp::n::length(set));
      ASSERT_EQ(inserted, sp::length(set));
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
        }
      }
      ASSERT_EQ(std::size_t(0), cnt);
    }

  } while (false);

  delete[] raw;
}

// #endif
