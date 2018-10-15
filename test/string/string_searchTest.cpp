#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <string/boyer_moore_search.h>
#include <string/knuth_morris_pratt_search.h>
#include <string/naive_search.h>
#include <util/Timer.h>
#include <util/array.h>

typedef const char *(*test_search_fp)(const char *text, std::size_t,
                                      const char *needle, std::size_t);

TEST(string_search, test) {
  const char *const text = "spooky";
  {
    const char *it = sp::kmp::search(text, "oo");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text + 2);
  }

  {
    const char *it = sp::kmp::search(text, "s");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::kmp::search(text, text);
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::kmp::search(text, "ok");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text + 3);
  }
}

static void
test_2(test_search_fp search) {
  // char it = '!';
  // const char end = '~' + char(1);

  char it = 'A';
  const char end = 'Z' + char(1);
  const std::size_t cap = end - it;

  sp::DynamicArray<char> text(cap);
  for (; it != end; ++it) {
    ASSERT_TRUE(push(text, it));
  }
  ASSERT_TRUE(is_full(text));

  const char *const b = text.data();
  printf("|%.*s|:%zu\n", length(text), b, length(text));

  for (std::size_t i = 2; i < length(text); ++i) {

    std::size_t k = i;
    while (true) {
      const char *const needle = b + k++;
      const std::size_t nlen = sp::distance(needle, text.end());

      // printf("nlen: %zu\n", nlen);
      if (nlen == 0) {
        break;
      }

      // {
      //   bool dasd = memcmp(b + 1, needle, 3) == 0;
      //   printf("[%.*s]:%zu|%s\n", len, needle, len, dasd ? "true" : "false");
      // }

      const char *it = search(b, length(text), needle, nlen);
      if (!it) {
        printf("i:%zu, len:%zu\n", i, nlen);
        printf("nedl[%.*s]:%zu\n", nlen, needle, nlen);
        printf("text[%.*s]:%zu\n", length(text), b, length(text));
      }
      ASSERT_TRUE(it);

      ASSERT_EQ(it, needle);
    }
  }
}

TEST(string_search, test_booyer_moore_search2) {
  test_2(sp::bm::search);
}

TEST(string_search, test_naive2) {
  test_2(sp::naive::search);
}

TEST(string_search, test_knuth_morris_pratt2) {
  test_2(sp::kmp::search);
}

static char
dist_rand_char(prng::xorshift32 &ra) {
  std::uint32_t max = (int(~char(0)) & int(0xff));
  assertx(max == 255);
  auto res = uniform_dist(ra, 0, max);
  assertxs(res < 256, res);
  return res;
}

static char
dist_rand_readable_char(prng::xorshift32 &ra) {
Lit:
  auto res = prng::uniform_dist(ra, '!', '~' + char(1));
  if (res == '"' || res == '\\') {
    goto Lit;
  }
  return res;
}

static void
test_large(sp::TimerContext &ctx, test_search_fp search) {
  prng::xorshift32 r(1);

  const std::size_t length = 25 * 1024 * 1;

  // printf("length[%zu]\n", length);
  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_rand_readable_char);

  // timer(ctx, [length, &r, text, search]() {
  const char *const end = text + length;

  std::size_t cnt = 0;
  for (std::size_t i = 0; i < length; ++i) {
    const std::size_t strt_idx = uniform_dist(r, 0, length - 1);

    const std::size_t n_cap = length - strt_idx;
    constexpr std::size_t wasd = 5;
    if (n_cap > wasd) {
      ++cnt;

      // if (cnt % 10000 == 0) {
      //   printf("cnt[%zu]\n", cnt);
      // }

      const auto n_len =
          uniform_dist(r, wasd, std::min(std::size_t(20), n_cap));

      const char *const needle = text + strt_idx;
      ASSERT_TRUE(needle < end);
      ASSERT_TRUE(needle + n_len <= end);
      // printf("needle[%.*s]: %zu\n", n_len, needle, n_len);

      const char *it = search(text, length, needle, n_len);
      ASSERT_TRUE(it);
      if (std::memcmp(it, needle, n_len) != 0) {
        printf("needle[%.*s]: %zu\n", n_len, needle, n_len);
        printf("found[%.*s]: %zu\n", n_len, it, n_len);
      }
      ASSERT_EQ(0, std::memcmp(it, needle, n_len));
      ASSERT_EQ(it, needle);
    }
    // printf("%s\n", it);
  }
  // });

  // printf("%s\n", text);

  delete[] text;
}

TEST(string_search, test_naive_large) {
  sp::TimerContext ctx;
  test_large(ctx, sp::naive::search);
}

TEST(string_search, test_boyer_moore_large) {
  // Array:
  // Average: 324 msec
  // Median: 327 msec
  sp::TimerContext ctx;
  // for (std::size_t i = 0; i < 100; ++i) {
  test_large(ctx, sp::bm::search);
  // }

  // print(ctx);
  // printf("Average: \n");
  // print(average(ctx));
  // printf("Median: \n");
  // print(median(ctx));
}

TEST(string_search, test_knuth_morris_pratt_large) {
  sp::TimerContext ctx;
  test_large(ctx, sp::kmp::search);
}

static void
test_subset(test_search_fp search) {
  prng::xorshift32 r(1);
  const std::size_t length = 40 * 1024 * 1;

  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_rand_char);

  const char *needle = text;
  const char *const end = text + length;

  std::size_t l_cmp = length;
  for (; needle != end && sp::distance(needle, end) > 5; ++needle) {
    const std::size_t n_len = sp::distance(needle, end);
    ASSERT_EQ(l_cmp, n_len);
    // printf("length[%zu]\n", l_cmp);
    // printf("[%zu]\n", n_len);
    const char *s = search(text, length, needle, n_len);
    ASSERT_TRUE(s);
    ASSERT_EQ(s, needle);
    --l_cmp;
  }

  delete[] text;
}

TEST(string_search, test_boyer_moore_subset) {
  test_subset(sp::bm::search);
}

TEST(string_search, test_naive_subset) {
  test_subset(sp::naive::search);
}

TEST(string_search, test_knuth_morris_pratt_subset) {
  test_subset(sp::kmp::search);
}

TEST(string_search, tsts_1) {
  const char *const text = ":|$2V7%CR'B(km^N5CB&|^Z@5frI@!nE";
  const char *const needle = "^N5CB&|^Z@5";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
}

TEST(string_search, tsts_2) {
  /*
   * last[a] =  0 shift = 8
   * last[f] = -1 shift = 9
   * last[e] = -1 shift = 9
   * last[a] =  0 shift = 8
   */
  const char *const text = "rlhlgh";
  const char *const needle = "lgh";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(0, std::memcmp(needle, it, std::strlen(needle)));
}

TEST(string_search, tsts_3) {
  /*
   * last[b] = -1, shfit = 1
   * last[a] =  0, shift = 8
   */
  const char *const text = "blgorithmalgorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(0, std::memcmp(needle, it, std::strlen(needle)));
}

TEST(string_search, tsts_4) {
  const char *const text = "algorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(it, text);
}

TEST(string_search, tsts_5) {
  const char *const text = "xalgorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  printf("res: %s\n", it);
  ASSERT_EQ(it, text + 1);
}

TEST(string_search, tsts_6) {
  const char *const text = "algorithmx";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(it, text);
}

TEST(string_search, tsts_7) {
  const char *const text = "wasddfghjklasd";
  const char *const needle = "lasd";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(0, std::memcmp(it, needle, std::strlen(needle)));
}

TEST(string_search, tsts_min) {
  /* last[e] =  4 shift = 1
   * last[f] = -1 shift = 5
   * last[t] = -1 shift = 5
   * last[3] =  3 shift = 1
   */
  // const char *const text = "ceefqwerty0123easd";
  // const char *const nedl = "0123e";

  const char *const text = "eefqwerty0123easd";
  const char *const nedl = "3e";

  const char *const it = sp::bm::search(text, nedl);
  ASSERT_TRUE(it);
}

#if 0
TEST(string_search, tsts_min_min) {
  prng::xorshift32 r(1);
  const std::size_t length = 25 * 1024 * 1;
  // const std::size_t length = 32;

  printf("length[%zu]\n", length);

  char *const text = new char[length + 1];
  const char *const text_end = text + length;

  std::size_t best_min = length;

  while (true) {
    const auto prng_state = r.state;
    text[length] = '\0';
    fill(r, text, length, dist_rand_char);

    const char *text_it = text;

    constexpr std::size_t needle_len = 11;
    char needle[needle_len + 1] = {'\0'};
    {
      std::size_t needle_offset = uniform_dist(r, 0, length - needle_len);
      const char *const needle_begin = (text + needle_offset);
      const char *const needle_end = needle_begin + needle_len;
      ASSERT_EQ(needle_len, sp::distance(needle_begin, needle_end));
      ASSERT_TRUE(needle_end <= text_end);
      memcpy(needle, needle_begin, needle_len);
    }

    const char *const cmp_it = sp::naive::search(text, needle);
    ASSERT_TRUE(cmp_it);

    const char *min_it = nullptr;
    for (; text_it < cmp_it; ++text_it) {
      const char *const it = sp::bm::search(text_it, needle);

      ASSERT_TRUE(sp::naive::search(text_it, needle));
      if (!it) {
        min_it = text_it;
      }

      //   ASSERT_TRUE(it);
      // ASSERT_EQ(it, text);
    } // for

    if (min_it) {
      auto min_len = std::strlen(min_it);
      if (min_len < best_min) {
        printf("---------------------------prng[%u]--\n", prng_state);
        best_min = min_len;
        printf("txt[%s]:%zu\n", min_it, min_len);
        printf("ptn[%s]:%zu\n", needle, std::strlen(needle));
      }
    }

  } // while
}
#endif

TEST(string_search, tsts_success_min_min) {
  prng::xorshift32 r(1);
  // prng::xorshift32 r(3284282817);
  const std::size_t length = 25 * 1024 * 1;
  // const std::size_t length = 32;

  printf("length[%zu]\n", length);
  std::size_t cmpers = 0;

  char *const text = new char[length + 1];
  const char *const text_end = text + length;

  while (true) {
    const auto prng_state = r.state;
    text[length] = '\0';
    fill(r, text, length, dist_rand_char);

    const char *text_it = text;

    constexpr std::size_t needle_len = 11;
    char needle[needle_len + 1] = {'\0'};
    {
      std::size_t needle_offset = uniform_dist(r, 0, length - needle_len);
      const char *const needle_begin = (text + needle_offset);
      const char *const needle_end = needle_begin + needle_len;
      ASSERT_EQ(needle_len, sp::distance(needle_begin, needle_end));
      ASSERT_TRUE(needle_end <= text_end);
      memcpy(needle, needle_begin, needle_len);
    }

    const char *const cmp_it = sp::naive::search(text, needle);
    ASSERT_TRUE(cmp_it);

    const char *min_it = nullptr;
    for (; text_it < cmp_it; ++text_it) {
      ++cmpers;
      if ((cmpers % 1000000) == 0) {
        printf("cmpers[%zu], seed[%u]\n", cmpers, prng_state);
      }
      const char *const it = sp::bm::search(text_it, needle);

      if (!it) {
        printf("seed[%u]\n", prng_state);
      }
      ASSERT_TRUE(it);
      ASSERT_EQ(0, std::memcmp(it, needle, needle_len));
    } // for

  } // while
}
