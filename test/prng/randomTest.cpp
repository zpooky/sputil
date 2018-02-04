#include "gtest/gtest.h"

// #include <prng/random.h>
#include <prng/util.h>
#include <prng/xorshift.h>

TEST(randomTest, test) {
  prng::Xorshift32 r(1);
  for (std::uint32_t inc = 0; inc < 100; ++inc) {
    for (std::uint32_t ex = inc + 1; ex < 100; ++ex) {
      // printf("#####%u, %u\n", inc, ex);

      bool first = false;
      bool last = false;
      for (std::size_t a = 0; a < 2000; ++a) {
        auto res = uniform_dist(r, inc, ex);
        // printf("%u >= inc[%u] && %u < ex[%u]\n", res, inc, res, ex);
        first |= res == inc;
        last |= res == ex;
        ASSERT_TRUE(res >= inc);
        ASSERT_TRUE(res < ex);
      }
      ASSERT_TRUE(first);
      ASSERT_FALSE(last);
    }
  }
}
