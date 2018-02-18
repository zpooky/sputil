#include "gtest/gtest.h"

#include <prng/URandom.h>
#include <prng/util.h>
#include <prng/xorshift.h>

#define test(r)                                                                \
  do {                                                                         \
    using TTT = typename decltype(r)::Word;                                    \
    for (TTT inc = 0; inc < 100; ++inc) {                                      \
      for (TTT ex = inc + 1; ex < 100; ++ex) {                                 \
        bool first = false;                                                    \
        bool last = false;                                                     \
        for (TTT a = 0; a < 1000; ++a) {                                       \
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
  } while (0)

#define test_uniform_one(r)                                                    \
  do {                                                                         \
    for (std::uint32_t i = 0; i < 160; ++i) {                                  \
      using TTT = typename decltype(r)::Word;                                  \
      TTT start(0);                                                            \
      TTT end(1);                                                              \
      auto res = uniform_dist(r, start, end);                                  \
      ASSERT_EQ(res, TTT(0));                                                  \
    }                                                                          \
  } while (0)

TEST(randomTest, test_xorshift32) {
  prng::Xorshift32 r(1);
  test(r);
}

TEST(randomTest, test_xorshift32_dist_limited) {
  prng::Xorshift32 r(1);
  test_uniform_one(r);
}

TEST(randomTest, test_xorshift128plus) {
  prng::Xorshift128plus r(1, 1);
  test(r);
}

TEST(randomTest, test_xorshift128plus_dist_limited) {
  prng::Xorshift128plus r(1, 1);
  test_uniform_one(r);
}

TEST(randomTest, test_seeded) {
  for (std::size_t i = 0; i < 40000; ++i) {
    auto r1 = prng::seed<prng::Xorshift32>();
    random(r1);
    auto r2 = prng::seed<prng::Xorshift128plus>();
    random(r2);
  }
}

/*
 * TEST(randomTest, ll) {
 *   prng::Xorshift32 r(1);
 *   for (std::size_t i = 0; i < 10000; ++i) {
 *     printf("%u,", random(r));
 *     if (i % 10 == 0) {
 *       printf("\n");
 *     }
 *   }
 * }
 */
