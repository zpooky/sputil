#include "gtest/gtest.h"
#include <util/Bitset.h>
#include <random>

TEST(BitsetTest, test_random) {
  constexpr std::size_t len = 128;
  constexpr std::size_t bits=len*sizeof(std::uint64_t)*8;

  std::uint64_t raw[len]={0};
  sp::Bitset b(raw);
  for(std::size_t i=0;i<bits;++i){
    ASSERT_FALSE(test(b,i));
  }

  std::size_t in[bits] = {0};
  for(std::size_t i =0;i<bits;++i){
    in[i] = i;
  }
  std::mt19937 g(0);
  std::shuffle(in, in + bits, g);

  for(std::size_t i=0;i<bits;++i){
    std::size_t testIdx = in[i];

    {
      ASSERT_FALSE(test(b,testIdx));
      bool old = set(b,testIdx, true);
      ASSERT_FALSE(old);
      ASSERT_TRUE(test(b,testIdx));
    }
    {
      bool old = set(b, testIdx, false);
      ASSERT_TRUE(old);
      ASSERT_FALSE(test(b,testIdx));
    }
    {
      bool newv = toggle(b,testIdx);
      ASSERT_TRUE(newv);
      ASSERT_TRUE(test(b,testIdx));
    }
  }

  for(std::size_t i=0;i<bits;++i){
    ASSERT_TRUE(test(b,i));
  }
}

TEST(BitsetTest, test_sequence) {
  constexpr std::size_t len = 128;
  constexpr std::size_t bits=len*sizeof(std::uint64_t)*8;

  std::uint64_t raw[len]={0};
  sp::Bitset b(raw);
  for(std::size_t i=0;i<bits;++i){
    ASSERT_FALSE(test(b,i));
  }

  for(std::size_t testIdx=0;testIdx<bits;++testIdx){
    // printf("=============idx[%zu]\n",testIdx);
    {
      ASSERT_FALSE(test(b,testIdx));
      bool old = set(b,testIdx, true);
      ASSERT_FALSE(old);
      ASSERT_TRUE(test(b,testIdx));
    }
    {
      bool old = set(b, testIdx, false);
      ASSERT_TRUE(old);
      ASSERT_FALSE(test(b,testIdx));
    }
    {
      bool newv = toggle(b,testIdx);
      ASSERT_TRUE(newv);
      ASSERT_TRUE(test(b,testIdx));
    }
  }

  for(std::size_t i=0;i<bits;++i){
    ASSERT_TRUE(test(b,i));
  }
}
