#include "gtest/gtest.h"
#include <buffer/CircularByteBuffer.h>
#include <cstring>
#include <encode/hex.h>
#include <prng/util.h>
#include <prng/xorshift.h>

template <std::size_t l>
static void
print_hex(const char *format, unsigned char (&buffer)[l]) {
  char msg[(l * 2) + 5] = {0};
  hex::encode(buffer, l, msg);
  printf(format, msg);
}

TEST(CircularByteBufferTest, test) {
  constexpr std::size_t cap = 16;
  sp::StaticCircularByteBuffer<cap> b;
  constexpr std::size_t rcap = 19;
  unsigned char raw[rcap];
  for (unsigned char i = 0; i < rcap; ++i) {
    raw[i] = i;
  }
  print_hex("writ[%s]\n", raw);

  ASSERT_EQ(std::size_t(0), length(b));
  ASSERT_TRUE(is_empty(b));
  ASSERT_FALSE(is_full(b));

  ASSERT_EQ(cap, push_back(b, raw));
  ASSERT_EQ(std::size_t(0), push_back(b, raw));

  ASSERT_EQ(cap, length(b));
  ASSERT_TRUE(is_full(b));
  ASSERT_FALSE(is_empty(b));

  {
    unsigned char read[rcap];
    ASSERT_EQ(cap, pop_front(b, read));
    print_hex("read[%s]\n", read);
    ASSERT_EQ(0, std::memcmp(read, raw, cap));
  }
}

TEST(CircularByteBufferTest, test_circular) {
  constexpr std::size_t cap = 16;
  constexpr std::size_t rcap = 19;
  sp::StaticCircularByteBuffer<cap> b;

  unsigned char raw[rcap];
  for (unsigned char i = 0; i < rcap; ++i) {
    raw[i] = i;
  }

  {
    ASSERT_EQ(std::size_t(2), push_back(b, raw, 2));
    ASSERT_EQ(std::size_t(2), length(b));
    ASSERT_FALSE(is_empty(b));
    ASSERT_FALSE(is_full(b));
  }

  {
    ASSERT_EQ(std::size_t(2), push_back(b, raw + 2, 2));
    ASSERT_EQ(std::size_t(4), length(b));
    ASSERT_FALSE(is_empty(b));
    ASSERT_FALSE(is_full(b));
  }

  {
    unsigned char read = 1;
    ASSERT_EQ(std::size_t(1), pop_front(b, &read, 1));
    ASSERT_EQ(std::size_t(3), length(b));
    ASSERT_FALSE(is_empty(b));
    ASSERT_FALSE(is_full(b));

    ASSERT_EQ(0, read);
  }

  {
    unsigned char read = 0;
    ASSERT_EQ(std::size_t(1), pop_front(b, &read, 1));
    ASSERT_EQ(std::size_t(2), length(b));
    ASSERT_FALSE(is_empty(b));
    ASSERT_FALSE(is_full(b));

    ASSERT_EQ(1, read);
  }

  {
    ASSERT_EQ(14, push_back(b, raw));
    ASSERT_EQ(std::size_t(0), push_back(b, raw));

    ASSERT_EQ(cap, length(b));
    ASSERT_TRUE(is_full(b));
    ASSERT_FALSE(is_empty(b));
  }

  {
    unsigned char read[rcap];
    ASSERT_EQ(cap, pop_front(b, read));
    print_hex("read[%s]\n", read);
    ASSERT_EQ(char(0), read[2]);
    ASSERT_EQ(char(1), read[3]);
    ASSERT_EQ(0, std::memcmp(read + 2, raw, 14));
  }
}

TEST(CircularByteBufferTest, test_pop_empty) {
  constexpr std::size_t sz = 16;
  {
    sp::StaticCircularByteBuffer<sz> b;
    unsigned char c = 99;
    ASSERT_EQ(0, pop_front(b, &c, 1));
    ASSERT_EQ(c, 99);

    unsigned char out[] = {0, 0};
    ASSERT_EQ(0, pop_front(b, out, sizeof(out)));
  }
  {

    sp::StaticCircularByteBuffer<sz> b;
    {
      unsigned char in = 127;
      ASSERT_EQ(1, push_back(b, &in, 1));

      unsigned char c = 99;
      ASSERT_EQ(1, pop_front(b, &c, 1));
      ASSERT_EQ(c, 127);
    }

    unsigned char c = 99;
    ASSERT_EQ(0, pop_front(b, &c, 1));
    ASSERT_EQ(c, 99);

    unsigned char out[] = {0, 0};
    ASSERT_EQ(0, pop_front(b, out, sizeof(out)));
  }
  {

    sp::StaticCircularByteBuffer<sz> b;
    unsigned char in[] = {13, 240};
    ASSERT_EQ(2, push_back(b, in, sizeof(in)));

    {
      unsigned char out[] = {0, 0};
      ASSERT_EQ(2, pop_front(b, out, sizeof(out)));
      ASSERT_EQ(out[0], 13);
      ASSERT_EQ(out[1], 240);
    }

    unsigned char c = 99;
    ASSERT_EQ(0, pop_front(b, &c, 1));
    ASSERT_EQ(c, 99);

    unsigned char out[] = {0, 0};
    ASSERT_EQ(0, pop_front(b, out, sizeof(out)));
  }
}

TEST(CircularByteBufferTest, test_random) {
  prng::xorshift32 r(1);
  std::size_t its = 0;
  while (its++ < 10000) {
    constexpr std::size_t sz = 16;
    unsigned char in[255];
    for (std::size_t i = 0; i < sizeof(in); ++i) {
      in[i] = i;
    }
    unsigned char cmp = 0;
    unsigned char *in_it = in;
    const unsigned char *const in_end = in + sizeof(in);
    //
    sp::StaticCircularByteBuffer<sz> b;
    while (in_it != in_end) {
      {
        std::size_t remaining_in(in_end - in_it);
        // assert(remaining_in > 0);
        std::size_t write_max = std::min(remaining_write(b), remaining_in);
        // assert(write_max > 0);
        auto write = uniform_dist(r, std::uint32_t(0),
                                  std::uint32_t(write_max) + 1); // TODO why +1
        // printf("write(%zu),rem_w(%zu),rem_r(%zu)\n", write,
        // remaining_write(b), remaining_read(b));

        auto written = push_back(b, in_it, write);
        ASSERT_EQ(written, write);
        in_it += written;
      }

      {
        unsigned char out[16] = {0};
        auto read_max = std::min(remaining_read(b), sizeof(out));
        // assert(read_max > 0);
        auto read =
            uniform_dist(r, std::uint32_t(0), std::uint32_t(read_max) + 1);
        // printf("read(%zu),rem_w(%zu),rem_r(%zu)\n", read, remaining_write(b),
        // remaining_read(b));
        auto readed = pop_front(b, out, read);
        ASSERT_EQ(readed, read);
        {
          for (std::size_t i = 0; i < readed; ++i) {
            ASSERT_EQ(out[i], cmp++);
          }
        }
      }
      // sleep(1);
    }

    {
      unsigned char out[16] = {0};
      auto readed = pop_front(b, out, sizeof(out));
      for (std::size_t i = 0; i < readed; ++i) {
        ASSERT_EQ(out[i], cmp++);
      }
      ASSERT_TRUE(is_empty(b));
      ASSERT_FALSE(is_full(b));
      ASSERT_EQ(std::size_t(0), length(b));
      ASSERT_EQ(std::size_t(0), remaining_read(b));
    }
    // printf("asd[%zu]\n", its++);
  } // while(true)
}

TEST(CircularByteBufferTest, test_read_buffer) {
  constexpr std::size_t sz = 8;
  sp::StaticCircularByteBuffer<sz> b;
  using AT = sp::StaticArray<std::tuple<unsigned char *, std::size_t>, 2>;

  {
    AT a;
    ASSERT_EQ(a.length, 0);
    ASSERT_TRUE(read_buffer(b, a));
    ASSERT_EQ(a.length, 0);
  }

  {
    for (unsigned char i = 0; i < sz; ++i) {
      ASSERT_EQ(push_back(b, &i, 1), 1);
      ASSERT_TRUE(!is_empty(b));
      ASSERT_EQ(i + 1, length(b));

      {
        AT a;
        ASSERT_TRUE(read_buffer(b, a));
        ASSERT_EQ(a.length, 1);

        auto d = get(a, 0);
        ASSERT_TRUE(d);
        ASSERT_EQ(b.buffer, std::get<0>(*d));
        ASSERT_EQ(i + 1, std::get<1>(*d));
      }
    }

    ASSERT_EQ(length(b), sz);
    ASSERT_TRUE(is_full(b));
  }

  for (unsigned char i = 0; i < sz / 2; ++i) {
    unsigned char c = 255;
    ASSERT_EQ(1, pop_front(b, &c, 1));
    ASSERT_EQ(c, i);
    {
      AT a;
      ASSERT_TRUE(read_buffer(b, a));
      ASSERT_EQ(a.length, 1);

      auto d = get(a, 0);
      ASSERT_TRUE(d);
      ASSERT_EQ(b.buffer + i + 1, std::get<0>(*d));
      ASSERT_EQ(length(b), std::get<1>(*d));
    }
  }

  for (unsigned char i = 0; i < (sz / 2) - 1; ++i) {
    ASSERT_EQ(push_back(b, &i, 1), 1);
    {
      AT a;
      ASSERT_TRUE(read_buffer(b, a));
      // ASSERT_EQ(a.length, 2);

      {
        std::size_t strt = 4;
        auto d = get(a, 0);
        ASSERT_TRUE(d);
        ASSERT_EQ(b.buffer + strt, std::get<0>(*d));
        ASSERT_EQ(i + 1 + strt, std::get<1>(*d));
      }
      {
        auto d = get(a, 1);
        ASSERT_TRUE(d);
        ASSERT_EQ(b.buffer, std::get<0>(*d));
        ASSERT_EQ(i + 1, std::get<1>(*d));
      }
    }
  }

  {
    const unsigned char i = 255;
    ASSERT_EQ(push_back(b, &i, 1), 1);

    AT a;
    ASSERT_TRUE(read_buffer(b, a));
    ASSERT_EQ(a.length, 1);

    auto d = get(a, 0);
    ASSERT_TRUE(d);
    ASSERT_EQ(b.buffer, std::get<0>(*d));
    ASSERT_EQ(sz, std::get<1>(*d));
  }
}
