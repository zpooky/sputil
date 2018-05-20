#include "gtest/gtest.h"
#include <random>
#include <util/Bitset.h>

template <typename Bits>
static void
BITSET_TEST_RAND(Bits &b, std::size_t bits) {
  for (std::size_t i = 0; i < bits; ++i) {
    ASSERT_FALSE(test(b, i));
  }

  auto in = new std::size_t[bits]{0};
  for (std::size_t i = 0; i < bits; ++i) {
    in[i] = i;
  }
  std::mt19937 g(0);
  std::shuffle(in, in + bits, g);

  for (std::size_t i = 0; i < bits; ++i) {
    for (std::size_t a = i; a < bits; ++a) {
      ASSERT_FALSE(test(b, in[a]));
    }

    std::size_t testIdx = in[i];
    {
      ASSERT_FALSE(test(b, testIdx));
      bool old = set(b, testIdx, true);
      ASSERT_FALSE(old);
      ASSERT_TRUE(test(b, testIdx));
    }
    {
      bool old = set(b, testIdx, false);
      ASSERT_TRUE(old);
      ASSERT_FALSE(test(b, testIdx));
    }
    {
      bool newv = toggle(b, testIdx);
      ASSERT_TRUE(newv);
      ASSERT_TRUE(test(b, testIdx));
    }

    for (std::size_t a = 0; a <= i; ++a) {
      ASSERT_TRUE(test(b, in[a]));
    }
  }

  for (std::size_t i = 0; i < bits; ++i) {
    ASSERT_TRUE(test(b, i));
  }
  delete[] in;
}

template <typename Bits>
static void
BITSET_TEST_SEQUENCE(Bits &b) {
  for (std::size_t i = 0; i < bits(b); ++i) {
    ASSERT_FALSE(test(b, i));
  }

  for (std::size_t testIdx = 0; testIdx < bits(b); ++testIdx) {
    {
      ASSERT_FALSE(test(b, testIdx));
      bool old = set(b, testIdx, true);
      ASSERT_FALSE(old);
      ASSERT_TRUE(test(b, testIdx));
    }
    {
      bool old = set(b, testIdx, false);
      ASSERT_TRUE(old);
      ASSERT_FALSE(test(b, testIdx));
    }
    {
      bool newv = toggle(b, testIdx);
      ASSERT_TRUE(newv);
      ASSERT_TRUE(test(b, testIdx));
    }
  }

  for (std::size_t i = 0; i < bits(b); ++i) {
    ASSERT_TRUE(test(b, i));
  }
}

static constexpr std::size_t t_len = 24;
static constexpr std::size_t t_bits = t_len * sizeof(std::uint64_t) * 8;
TEST(BitsetTest, test_random) {

  std::uint64_t raw[t_len] = {0};
  sp::Bitset b(raw);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_static_random) {
  sp::StaticBitset<t_len> b;
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random) {
  sp::SparseBitset b(1, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random2) {
  sp::SparseBitset b(2, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random4) {
  sp::SparseBitset b(4, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random8) {
  sp::SparseBitset b(8, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random16) {
  sp::SparseBitset b(16, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random32) {
  sp::SparseBitset b(32, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random64) {
  sp::SparseBitset b(64, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random128) {
  sp::SparseBitset b(128, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sparse_random256) {
  sp::SparseBitset b(256, t_len);
  BITSET_TEST_RAND(b, t_bits);
}

TEST(BitsetTest, test_sequence) {
  std::uint64_t raw[128] = {0};
  sp::Bitset b(raw);
  BITSET_TEST_SEQUENCE(b);
}

TEST(BitsetTest, test_static_sequence) {
  sp::StaticBitset<128> b;
  BITSET_TEST_SEQUENCE(b);
}

TEST(BitsetTest, test_sparse_sequence) {
  sp::SparseBitset b(1, 128);
  BITSET_TEST_SEQUENCE(b);
}
