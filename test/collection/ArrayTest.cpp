#include "gtest/gtest.h"
#include <collection/Array.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <sort/util.h>

template <std::size_t N>
static void
print_arr(const sp::UinStaticArray<std::size_t, N> &sut) {
  for (std::size_t i = 0; i < length(sut); ++i) {
    printf("%zu, ", sut[i]);
  }
  printf("\n");
}

TEST(ArrayTest, test_empty) {
  sp::Array<int> a;
  ASSERT_EQ(insert(a, 1), nullptr);
  ASSERT_EQ(get(a, 0), nullptr);
}

TEST(ArrayTest, test) {
  int buffer[10];
  sp::Array<int> a(buffer);
  {
    int *res = insert(a, 1);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 0);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 1);
    ASSERT_FALSE(res);
  }
  prng::xorshift32 r(1);
  shuffle(r, a);
  {
    int *res = get(a, 0);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, 1);
  }
  {
    int *res = get(a, 1);
    ASSERT_FALSE(res);
  }
}

struct ArrayTestData {
  void *ptr;
  int i;
  ArrayTestData()
      : ptr(nullptr)
      , i(-1) {
  }

  explicit ArrayTestData(int b)
      : ptr(nullptr)
      , i(b) {
  }
};

static void
swap(ArrayTestData &f, ArrayTestData &s) noexcept {
  sp::swap(f.i, s.i);
}

TEST(ArrayTest, test_data) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> a(buffer);
  sp::Array<ArrayTestData> b;
  swap(a, b);

  for (int i = 0; i < len; ++i) {
    ArrayTestData in(i);
    auto *res = insert(b, in);
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }

  prng::xorshift32 r(1);
  for (int i = 0; i < int(b.length); ++i) {
    auto *c = get(b, std::size_t(i));
    ASSERT_TRUE(c);
    // printf("%d,", c->i);
  }
  // printf("\n");
  shuffle(r, b);
  for (int i = 0; i < int(b.length); ++i) {
    auto *c = get(b, std::size_t(i));
    ASSERT_TRUE(c);
    // printf("%d,", c->i);
  }
  // printf("\n");
}

TEST(ArrayTest, test_remove) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> b(buffer);

  for (int i = 0; i < len; ++i) {
    auto *res = insert(b, ArrayTestData(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }
  ASSERT_FALSE(insert(b, 11));

  int cnt = 0;
  while (b.length > 0) {
    ASSERT_TRUE(remove(b, 0));
    ++cnt;
  }
  ASSERT_EQ(cnt, len);
}

TEST(ArrayTest, test_take) {
  constexpr int len = 10;
  ArrayTestData buffer[len];
  sp::Array<ArrayTestData> b(buffer);

  for (int i = 0; i < len; ++i) {
    auto *res = insert(b, ArrayTestData(i));
    ASSERT_TRUE(res);
    ASSERT_EQ(res->i, i);
  }
  ASSERT_FALSE(insert(b, 11));

  int cnt = 0;
  ArrayTestData cmp(0);
  while (b.length > 0) {
    ArrayTestData out(9999);
    ASSERT_TRUE(take(b, std::size_t(0), out));
    // printf("take: %d\n", out.i);
    ASSERT_EQ(cmp.i, out.i);
    cmp.i = b.length;
    ++cnt;
  }
  ASSERT_EQ(cnt, len);
}

TEST(ArrayTest, test_binary_search) {
  constexpr std::size_t cap = 1024 * 1;
  sp::UinStaticArray<std::size_t, cap> a;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_EQ(a.length, i);

    for (std::size_t k = 0; k < cap; ++k) {
      const auto *res = bin_search(a, k);
      // auto *res = find(a, [&k](auto c) { return c == k; });
      // bin_search 10x faster than find
      if (k < i) {
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, k);
      } else {
        ASSERT_FALSE(res);
      }
    }

    auto *res = insert(a, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
}

TEST(ArrayTest, test_binary_insert) {
  constexpr std::size_t cap = 1000 * 1;
  prng::xorshift32 r(1);
  // while (true) {

  sp::UinStaticArray<std::size_t, cap> a;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_EQ(a.length, i);

    auto *res = insert(a, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);

    for (std::size_t k = 0; k <= i; ++k) {
      auto *sres = bin_search(a, k);
      ASSERT_TRUE(sres);
      ASSERT_EQ(*sres, k);
    }
  }

  shuffle(r, a);
  // printf("shuffled: ");
  // for_each(a, [](auto it) {
  //   #<{(||)}>#
  //   printf("%zu,", it);
  // });
  // printf("\n");

  {
    sp::UinStaticArray<std::size_t, cap> next;
    for (std::size_t i = 0; i < cap; ++i) {
      ASSERT_EQ(next.length, i);

      const auto ins = *get(a, i);
      // printf("insert(%zu)\n", ins);
      auto *res = bin_insert(next, ins);
      {
        // for_each(next, [](auto it) {
        //   #<{(||)}>#
        //   printf("%zu,", it);
        // });
        // printf("\n");
      }

      ASSERT_TRUE(res);
      ASSERT_EQ(*res, ins);
      for (std::size_t k = 0; k <= i; ++k) {
        auto *sres = bin_search(next, ins);
        ASSERT_TRUE(sres);
        ASSERT_EQ(*sres, ins);
      }
    }
    // {
    //   for_each(next, [](auto it) {
    //     #<{(||)}>#
    //     printf("%zu,", it);
    //   });
    //   printf("\n");
    // }
    for (std::size_t i = 0; i < cap; ++i) {
      auto *res = get(next, i);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, i);
    }
    ASSERT_FALSE(get(next, cap));
    {
      for (std::size_t i = 0; i < cap; ++i) {
        const auto ins = a[i];
        ASSERT_TRUE(bin_remove(next, ins));
        ASSERT_EQ(nullptr, bin_search(next, ins));
        ASSERT_FALSE(bin_remove(next, ins));
      }
      ASSERT_TRUE(is_empty(next));
      ASSERT_EQ(length(next), std::size_t(0));
    }
  }
  // }
}

TEST(ArrayTest, test_binary) {
  constexpr std::size_t cap = 1024;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> a;
  for (std::size_t it = 0; it < 1; ++it) {
    {
      for (std::size_t i = 0; i < cap; ++i) {
        ASSERT_EQ(a.length, i);
        auto *res = insert(a, i);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, i);
      }
      for (std::size_t i = 0; i < cap; ++i) {
        auto *res = get(a, i);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, i);
      }
      shuffle(r, a);
      ASSERT_EQ(cap, a.length);
      ASSERT_FALSE(insert(a, 999999));
      ASSERT_EQ(cap, a.length);
    }

    {
      sp::UinStaticArray<std::size_t, cap> dest;
      for (std::size_t i = 0; i < cap; ++i) {
        for (std::size_t k = 0; k < cap; ++k) {
          auto c = get(a, k);
          auto *res = bin_search(dest, *c);
          if (k < i) {
            ASSERT_TRUE(res);
            ASSERT_EQ(*res, *c);
          } else {
            if (res) {
              printf("search(%zu) -> got(%zu)\n", *c, *res);
            }
            ASSERT_FALSE(res);
          }
        }
        /**/
        auto c = get(a, i);
        ASSERT_TRUE(c);
        auto *res = bin_insert(dest, *c);
        ASSERT_TRUE(res);
        ASSERT_EQ(*res, *c);
      }
    }
    {
      ASSERT_EQ(cap, a.length);
      clear(a);
      ASSERT_EQ(std::size_t(0), a.length);
    }
  }
}

TEST(ArrayTest, test_bin_insert_unique) {
  constexpr std::size_t cap = 1024;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> in;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_TRUE(insert(in, i));
  }
  shuffle(r, in);

  sp::UinStaticArray<std::size_t, cap> sut;
  for (std::size_t i = 0; i < cap; ++i) {
    auto ires = bin_insert_unique(sut, in[i]);
    ASSERT_TRUE(ires);
    auto sres = bin_search(sut, in[i]);
    ASSERT_TRUE(sres);

    ASSERT_EQ(ires, sres);
    ASSERT_EQ(*ires, *sres);

    ASSERT_EQ(in[i], *ires);
    ASSERT_EQ(in[i], *sres);
  }

  for (std::size_t i = 0; i < cap; ++i) {
    auto sres = bin_search(sut, in[i]);
    ASSERT_TRUE(sres);

    ASSERT_EQ(in[i], *sres);
  }

  for (std::size_t i = 0; i < cap; ++i) {
    const auto lb = length(sut);
    auto ires = bin_insert_unique(sut, in[i]);
    ASSERT_FALSE(ires);
    ASSERT_EQ(lb, length(sut));
  }

  for (std::size_t i = 0; i < cap; ++i) {
    auto sres = bin_search(sut, in[i]);
    ASSERT_TRUE(sres);

    ASSERT_EQ(in[i], *sres);
  }

  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_EQ(sut[i], i);
  }

  ASSERT_TRUE(is_full(sut));
  for (std::size_t a = 0; a < cap; ++a) {
    ASSERT_TRUE(bin_remove(sut, in[a]));

    for (std::size_t i = 0; i < a; ++i) {
      auto sres = bin_search(sut, in[i]);
      ASSERT_FALSE(sres);
    }

    for (std::size_t i = a + 1; i < cap; ++i) {
      auto sres = bin_search(sut, in[i]);
      ASSERT_TRUE(sres);

      ASSERT_EQ(in[i], *sres);
    }
  }
  ASSERT_TRUE(is_empty(sut));
}

TEST(ArrayTest, test_bin_insert_unique2) {
  constexpr std::size_t cap = 1024;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> in;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_TRUE(insert(in, i));
  }
  shuffle(r, in);

  sp::UinStaticArray<std::size_t, cap> sut;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_EQ(i, length(sut));
    {
      auto res = bin_insert(sut, in[i]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
    }

    {
      auto res = bin_search(sut, in[i]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
    }

    {
      const auto lb = length(sut);
      if (bin_insert_unique(sut, in[i])) {

        ASSERT_FALSE(true);
      }
      ASSERT_EQ(lb, length(sut));
    }

    { ASSERT_TRUE(bin_remove(sut, in[i])); }

    {
      const auto lb = length(sut);
      auto res = bin_insert_unique(sut, in[i]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
      ASSERT_EQ(lb + 1, length(sut));
    }
  }
}

TEST(ArrayTest, test_bin_insert_unique3) {
  constexpr std::size_t cap = 1024;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> in;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_TRUE(insert(in, i));
  }
  shuffle(r, in);

  sp::UinStaticArray<std::size_t, cap> sut;
  for (std::size_t i = 0; i < cap; ++i) {
    // printf("%zu|", i);
    // print_arr(sut);

    for (std::size_t a = 0; a < i; ++a) {
      auto res = bin_insert_unique(sut, in[a]);
      if (res) {
        printf("fail|");
        print_arr(sut);
      }
      ASSERT_FALSE(res);
    }
    auto res = bin_insert_unique(sut, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
    if (!sp::is_sorted(sut.data(), length(sut))) {
      printf("is_sorted|");
      print_arr(sut);
      ASSERT_TRUE(false);
    }
  }
  for (std::size_t i = 0; i < cap; ++i) {
    auto res = bin_search(sut, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
  }
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_EQ(sut[i], i);
  }
}

TEST(ArrayTest, test_bin_insert3) {
  constexpr std::size_t cap = 256;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> in;
  for (std::size_t i = 0; i < cap; ++i) {
    ASSERT_TRUE(insert(in, i));
  }
  shuffle(r, in);

  sp::UinStaticArray<std::size_t, cap> sut;
  for (std::size_t i = 0; i < cap; ++i) {
    while (!is_full(sut)) {
      auto res = bin_insert(sut, in[i]);
      ASSERT_TRUE(res);
      ASSERT_EQ(*res, in[i]);
    }

    {
      bool res = false;
      do {
        res = bin_remove(sut, in[i]);
      } while (res);
    }

    auto res = bin_insert(sut, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
  }
  for (std::size_t i = 0; i < cap; ++i) {
    auto res = bin_search(sut, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
  }

  // for (std::size_t i = 0; i < length(sut); ++i) {
  //   printf("%zu, ", sut[i]);
  // }
  // printf("\n");
}

TEST(ArrayTest, test_insert_at) {
  constexpr std::size_t cap = 1024;
  prng::xorshift32 r(1);

  sp::UinStaticArray<std::size_t, cap> in;
  sp::UinStaticArray<std::size_t, cap * 2> sut;
  for (std::size_t i = 0; i < cap; ++i) {
    insert(in, i);
    insert(sut, 0);
  }
  // shuffle(r, in);
  // {
  //   printf("in : ");
  //   for (std::size_t i = 0; i < length(in); ++i) {
  //     printf("%zu-", in[i]);
  //   }
  //   printf("\n");
  // }

  for (std::size_t i = 0; i < length(in); ++i) {
    std::size_t idx = std::min(in[i], length(sut));
    auto res = insert_at(sut, idx, in[i]);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, in[i]);
  }
  // {
  //   printf("sut: ");
  //   for (std::size_t i = 0; i < cap; ++i) {
  //     printf("%zu-", sut[i]);
  //   }
  //   printf("\n");
  // }
  for (std::size_t i = 0; i < cap; ++i) {
    auto res = get(sut, i);
    ASSERT_TRUE(res);
    ASSERT_EQ(*res, i);
  }
}
