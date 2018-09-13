#include <collection/Array.h>
#include <gtest/gtest.h>
#include <map/HashSetProbing.h>
#include <prng/xorshift.h>

TEST(HashMapProbingTest, test) {
  constexpr int range = 1000;
  sp::HashSetProbing<int> set;
  prng::xorshift32 r(1);

  for (std::size_t a = 0; a < 10; ++a) {
    sp::DynamicArray<int> ref(25);

    for (int i = 0; i < range; ++i) {
      ASSERT_FALSE(lookup(set, i));

      int *maybe = insert(set, i);
      int *l = lookup(set, i);
      if (maybe) {
        ASSERT_TRUE(push(ref, i));
        // printf("%d\n", *maybe);

        ASSERT_TRUE(l);
        ASSERT_EQ(maybe, l);
        ASSERT_EQ(*maybe, *l);
        ASSERT_EQ(*maybe, i);
      } else {
        ASSERT_FALSE(l);
      }
    }

    ASSERT_EQ(length(ref), capacity(set));

    shuffle(r, ref);

    for_each(ref, [&set](int &in) {
      int *l = lookup(set, in);
      ASSERT_TRUE(l);
      ASSERT_EQ(*l, in);

      ASSERT_TRUE(remove(set, in));
      ASSERT_FALSE(lookup(set, in));
    });

    for (int i = 0; i < range; ++i) {
      ASSERT_FALSE(lookup(set, i));
      ASSERT_FALSE(remove(set, i));
    }
  }

  // printf("length:%zu\n", length);
}
