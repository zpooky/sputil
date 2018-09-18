// #define NDEBUG

#include <collection/Array.h>
#include <gtest/gtest.h>
#include <map/HashSet.h>
#include <map/HashSetProbing.h>
#include <prng/xorshift.h>
#include <util/Bitset.h>
#include <util/Timer.h>

template <typename SET>
static void
run_bench(SET &set) noexcept {
  constexpr int range = 1024 * 4;
  // constexpr int range = 350;
  // constexpr int range = 1024 * 1;
  prng::xorshift32 r(1);
  // for (std::size_t range = 471; range < 1024 * 4; ++range) //
  {
    sp::TimerContex ctx;
    // printf("range(%zu)\n", range);
    for (std::size_t a = 0; a < 1; ++a) //
    {
      sp::timer(ctx, [&]() {
        sp::DynamicArray<int> ref(range);

        for (int i = 0; i < range; ++i) {
          const int cmp = i;
          ASSERT_FALSE(lookup(set, i));
          ASSERT_EQ(cmp, i);

          // printf("insert(%d){", i);
          int *maybe = insert(set, i);
          ASSERT_EQ(cmp, i);
          // printf("}\n");

          int *l = lookup(set, i);
          ASSERT_EQ(cmp, i);
          if (maybe) {
            ASSERT_TRUE(push(ref, i));
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
            const int cmp = in;

            int *l = lookup(set, in);
            ASSERT_EQ(cmp, in);

            if (!l) {
              printf("length(%zu)\n", length(ref));
              printf("lookup(%d): null\n", in);
            }

            ASSERT_TRUE(l);
            ASSERT_EQ(*l, in);
          }
        }

        // ASSERT_EQ(length(ref), sp::rec::length(set));

        sp::rec::verify(set);
        shuffle(r, ref);

        for_each(ref, [&set](int &in) {
          const int cmp = in;
          int *l = lookup(set, in);
          ASSERT_EQ(cmp, in);

          if (!l) {
            printf("lookup(%d): null\n", in);
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

        // ASSERT_EQ(std::size_t(0), sp::rec::length(set));

        // printf("----\n");
        // avl::dump(set.tree);
      });
    }
  }
  /*
   * print(ctx);

   * printf("Average: ");
   * print(average(ctx));

   * printf("Median: ");
   * print(median(ctx));
   */

  // printf("length:%zu\n", length);
}

TEST(HashSetProbingTest, test_HashSetProbing) {
  sp::HashSetProbing<int> set;
  run_bench(set);
  // std::size_t cnt = 0;
  // for (std::size_t i = 0; i < set.capacity; ++i) {
  //   if (set.table[i].tag != sp::impl::HSPTag::EMPTY) {
  //     ++cnt;
  //     // ASSERT_EQ(set.table[i].tag, sp::impl::HSPTag::EMPTY);
  //   }
  // }
  // ASSERT_EQ(std::size_t(0), cnt);
}

TEST(HashSetProbingTest, test_HashSetTree) {
  sp::HashSet<int> set;
  run_bench(set);
}

TEST(HashSetProbingTest, test_rand) {
  prng::xorshift32 r(1);

  constexpr std::uint32_t range = 1024 * 9;
  constexpr std::size_t bits(sizeof(std::uint64_t) * 8);
  ASSERT_TRUE(range % bits == 0);
  const std::size_t length = range / bits;

  auto raw = new uint64_t[length];
  ASSERT_TRUE(raw);

  std::size_t inserted = 0;
  do {
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

      ASSERT_EQ(inserted, found);

      const std::uint32_t current = uniform_dist(r, 0, range);
      if (current == 1347) {
        printf("##########################[%u]\n", current);
      }

      const std::uint32_t *const l_before = lookup(set, current);
      const std::uint32_t *const res = insert(set, current);
      const std::uint32_t *const l_after = lookup(set, current);

      ASSERT_FALSE(insert(set, current));

      const bool already = test(present, std::size_t(current));
      if (already) {
        printf("dup[%u]\n", current);
        if (res) {
          printf("bad[%u]\n", current);
        }

        ASSERT_FALSE(res);
        ASSERT_TRUE(l_before);
        ASSERT_TRUE(l_after);

        {
          ASSERT_TRUE(remove(set, current));
          ASSERT_FALSE(lookup(set, current));

          --inserted;
          ASSERT_TRUE(sp::set(present, std::size_t(current), false));
          ASSERT_FALSE(sp::test(present, std::size_t(current)));
          ASSERT_FALSE(sp::set(present, std::size_t(current), false));
        }
      } else {
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

    ASSERT_TRUE(sp::rec::verify(set));

  } while (false);

  delete[] raw;
}
