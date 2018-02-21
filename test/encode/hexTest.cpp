#include "gtest/gtest.h"
#include <encode/hex.h>
#include <encode/hex.h>
#include <prng/util.h>
#include <prng/xorshift.h>

TEST(hexTest, test) {
  unsigned char raw_in[512] = {};
  prng::xorshift32 r(1);

  for (std::size_t i = 0; i < 30000; ++i) {
    std::size_t raw_in_length =
        uniform_dist(r, std::uint32_t(1), std::uint32_t(sizeof(raw_in)));
    fill(r, raw_in);

    char msg[(sizeof(raw_in) * 2) + 1] = {0};
    std::size_t msg_length = sizeof(msg);
    ASSERT_TRUE(hex::encode(raw_in, raw_in_length, msg, msg_length));
    // printf("f[%s]\n", msg);

    unsigned char raw_out[sizeof(raw_in)] = {0};
    std::size_t out_length = sizeof(raw_out);
    ASSERT_TRUE(hex::decode(msg, raw_out, out_length));

    {
      char debug[(sizeof(raw_in) * 2) + 1] = {0};
      ASSERT_TRUE(hex::encode(raw_out, out_length, debug));
      // printf("d[%s]\n", debug);
      ASSERT_EQ(std::strlen(msg), std::strlen(debug));
      ASSERT_EQ(0, std::memcmp(msg, debug, std::strlen(msg)));
    }
    ASSERT_EQ(raw_in_length, out_length);
    ASSERT_EQ(0, std::memcmp(raw_out, raw_in, out_length));
  }
}

TEST(hexTest, test_decode) {
  unsigned char out[512] = {0};
  std::size_t out_length = sizeof(out);

  const char *in = "0123456789abcdefABCDEF";
  const char *in_cal = "0123456789ABCDEFABCDEF";

  ASSERT_TRUE(hex::decode(in, out, out_length));
  ASSERT_EQ(std::strlen(in), std::size_t(22));
  ASSERT_EQ(out_length, std::size_t(11));

  char outraw[256] = {0};
  std::size_t outraw_length = sizeof(outraw);
  ASSERT_TRUE(hex::encode(out, out_length, outraw, outraw_length));
  ASSERT_EQ(outraw_length, std::strlen(in));

  // printf("in[%s]\nut[%s]\n", in, outraw);
  ASSERT_EQ(std::memcmp(in_cal, outraw, outraw_length), 0);
}
