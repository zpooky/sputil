// #define NDEBUG

#include <collection/Array.h>
#include <gtest/gtest.h>
#include <map/HashSet.h>
#include <map/HashSetProbing.h>
#include <prng/xorshift.h>
#include <util/Timer.h>

template <typename SET>
static void
run_bench(SET &set) noexcept {
  // constexpr int range = 1024 * 10;
  constexpr int range = 1024 * 4;
  prng::xorshift32 r(1);
  sp::TimerContex ctx;

  for (std::size_t a = 0; a < 10; ++a) //
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
  print(ctx);

  printf("Average: ");
  print(average(ctx));

  printf("Median: ");
  print(median(ctx));

  // printf("length:%zu\n", length);
}

TEST(HashMapProbingTest, test_HashSetProbing) {
  sp::HashSetProbing<int> set;
  run_bench(set);
}

TEST(HashMapProbingTest, test_HashSetTree) {
  sp::HashSet<int> set;
  run_bench(set);
}
