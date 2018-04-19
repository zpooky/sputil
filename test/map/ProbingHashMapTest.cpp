#include "gtest/gtest.h"
#include <collection/Array.h>
#include <map/ProbingHashMap.h>
#include <prng/xorshift.h>

TEST(ProbingHashMapTest, test_map) {
  sp::ProbingHashMap<int, int> h;
}

TEST(ProbingHashMapTest, test_static_set) {
  sp::UinStaticArray<std::size_t, 1024> arr;
  for (std::size_t i = 0; i < capacity(arr); ++i) {
    ASSERT_TRUE(insert(arr, i));
  }
  prng::xorshift32 r(1);
  shuffle(r, arr);

  sp::StaticProbingHashSet<std::size_t, 1024> set(arr);
  for (std::size_t i = 0; i < length(arr); ++i) {
    auto *res = lookup(set, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
}
