#include "gtest/gtest.h"

// #include <prng/random.h>
#include <prng/util.h>
#include <prng/xorshift.h>

#define test(r)                                                                \
do {                                                                           \
    for (std::uint32_t inc = 0; inc < 100; ++inc) {                            \
      for (std::uint32_t ex = inc + 1; ex < 100; ++ex) {                       \
        bool first = false;                                                    \
        bool last = false;                                                     \
        for (std::size_t a = 0; a < 2000; ++a) {                               \
          auto res = uniform_dist(r, inc, ex);                                 \
          first |= res == inc;                                                 \
          last |= res == ex;                                                   \
          ASSERT_TRUE(res >= inc);                                             \
          ASSERT_TRUE(res < ex);                                               \
        }                                                                      \
        ASSERT_TRUE(first);                                                    \
        ASSERT_FALSE(last);                                                    \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  while (0)

TEST(randomTest, test_xorshift32) {
  prng::Xorshift32 r(1);
  test(r);
}

TEST(randomTest, test_xorshift128plus) {
  prng::Xorshift128plus r(1, 1);
  test(r);
}
