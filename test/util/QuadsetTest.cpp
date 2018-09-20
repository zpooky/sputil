#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <util/Quadset.h>

TEST(Quadset, test) {
  prng::xorshift32 r(1);

  const std::size_t bits = sizeof(std::uint64_t) / 2;
  std::uint64_t buf[53] = {0};
  memset(buf, 0, sizeof(buf));
  constexpr std::size_t cap = sizeof(buf) * bits;

  std::uint8_t storage[cap] = {0};
  memset(storage, 0, sizeof(storage));

  // printf("cap[%zu]\n", cap);

  sp::Quadset q(buf);

  for (std::size_t cnt = 0; cnt < 1000; ++cnt) {
    for (std::size_t idx = 0; idx < cap; ++idx) {
      std::uint8_t priv = test(q, idx);
      ASSERT_EQ(priv, storage[idx]);

      const std::uint32_t current = uniform_dist(r, 0, 3 + 1);
      const std::uint8_t c(current);

      priv = set(q, idx, c);
      ASSERT_EQ(priv, storage[idx]);
      storage[idx] = c;
    }
    // printf("cnt[%zu]\n", cnt);
  }

  // for (std::size_t idx = 0; idx < cap; ++idx) {
  //   std::uint8_t priv = test(q, idx);
  //   ASSERT_EQ(priv, 0);
  //
  //   priv = set(q, idx, std::uint8_t(1));
  //   ASSERT_EQ(priv, 0);
  //   ASSERT_EQ(test(q, idx), 1);
  //
  //   priv = set(q, idx, std::uint8_t(2));
  //   ASSERT_EQ(priv, 1);
  //   ASSERT_EQ(test(q, idx), 2);
  //
  //   priv = set(q, idx, std::uint8_t(3));
  //   ASSERT_EQ(priv, 2);
  //   ASSERT_EQ(test(q, idx), 3);
  //
  //   priv = set(q, idx, std::uint8_t(0));
  //   ASSERT_EQ(priv, 3);
  //   ASSERT_EQ(test(q, idx), 0);
  // }
}
