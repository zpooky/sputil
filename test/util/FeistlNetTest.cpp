#include "gtest/gtest.h"
#include <cstdint>
#include <cstddef>
#include <util/Bitset.h>

/*
 * # Feistel Network
 * The same input always produce the same output
 * All unique input produce unique output
 * The transformation is always invertible [A -> B] & [B -> A]
 *
 * ## ref
 * http://antirez.com/news/113
 *
 * ## Procedure
 * - split plaintext block into two equal parts L,R
 * - For each round
 *   - ...
 * - Concat L,R
 */
std::uint16_t feistelNet(std::uint16_t input) {
  std::uint8_t l ( input & 0xff);
  std::uint8_t r  ((input >> 8)&0xff);
  for (std::size_t i = 0; i < 8; i++) {
    std::uint8_t nl = r;
    std::uint8_t F = (((r * 11) + (r >> 5) + 7 * 127) ^ r) & 0xff;
    r = l ^ F;
    l = nl;
  }
  return std::uint16_t(((r<<8)|l) & 0xffff);
}

TEST(FeistelNetTest, test_for) {
  constexpr std::size_t cap = 1025;
  auto buffer = new std::uint64_t[cap];
  memset(buffer,0,cap*sizeof(std::uint64_t));
  ASSERT_TRUE(buffer);
  sp::Bitset b(buffer,cap);

  std::uint16_t max = ~std::uint16_t(0);
  for(std::size_t i=0;i<= std::size_t(max);++i) {
    std::uint16_t in(i);
    ASSERT_EQ(i, std::size_t(in));

    std::uint16_t out = feistelNet(in);
    // printf("out: %u\n",out);
    ASSERT_FALSE(test(b,std::size_t(out)));
    ASSERT_FALSE(set(b,std::size_t(out),true));
    ASSERT_TRUE(test(b,std::size_t(out)));
  }

  for(std::size_t i=0;i<std::size_t(max);++i){
    // if(!test(b,i)) {
    //   printf("idx[%zu]\n",i);
    // }

    ASSERT_TRUE(test(b,i));
  }
  delete[]buffer;
}

TEST(FeistelNetTest, test_invertible) {
  std::uint16_t max = ~std::uint16_t(0);
  for(std::size_t i=0;i<= std::size_t(max);++i) {
    std::uint16_t in(i);
    std::uint16_t out = feistelNet(in);
    std::uint16_t back = feistelNet(out);

    ASSERT_EQ(in,back);
  }
}

// TEST(FeistelNetTest, test_while) {
//   std::size_t cnt = 0;
//   const std::uint16_t start =feistelNet(0);
//   std::uint16_t out =start;
//   do {
//     out = feistelNet(out);
//     ++cnt;
//   }while(out != start);
//   printf("cnt: %zu\n",cnt);
// }
