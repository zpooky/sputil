#include "gtest/gtest.h"
#include <hash/djb2.h>
#include <hash/fnv.h>
#include <util/Bloomfilter.h>

TEST(BloomFilterTest, test) {
  auto djb = [](const int &i) -> std::size_t {
    return djb2a::encode32(&i, sizeof(i));
  };
  auto fnv1a = [](const int &i) -> std::size_t {
    return fnv_1a::encode64(&i, sizeof(i));
  };

  sp::StaticArray<sp::hasher<int>, 2> h;
  assert(insert(h, djb));
  assert(insert(h, fnv1a));

  sp::BloomFilter<int, 1024> bfilter(h);

  for (int i = 0; i < 2024; ++i) {
    ASSERT_FALSE(test(bfilter, i));
    ASSERT_TRUE(insert(bfilter, i));
    ASSERT_TRUE(test(bfilter, i));
  }
}
