#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <string/boyer_moore_search.h>
#include <string/naive_search.h>
#include <util/array.h>

typedef const char *(*test_search_fp)(const char *text, std::size_t,
                                      const char *needle, std::size_t);

TEST(string_search, test) {
  const char *const text = "spooky";
  {
    const char *it = sp::bm::search(text, "oo");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text + 2);
  }

  {
    const char *it = sp::bm::search(text, "s");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::bm::search(text, text);
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::bm::search(text, "ok");
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

static char
dist_xx(prng::xorshift32 &ra) {
  return prng::uniform_dist(ra, '!', '~' + char(1));
}

static void
test_large(test_search_fp search) {
  prng::xorshift32 r(1);
  // const std::size_t length = 25 * 1024;
  const std::size_t length = 32;

  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_xx);

  for (std::size_t i = 0; i < 2000; ++i) {
    const std::size_t strt_idx = uniform_dist(r, 0, length - 1);

    const std::size_t cap = length - strt_idx;
    if (cap > 5) {
      const auto n_len = uniform_dist(r, 5, std::min(std::size_t(20), cap));

      const char *needle = text + strt_idx;
      // printf("needle[%.*s]: %zu\n", n_len, needle, n_len);

      const char *it = search(text, length, needle, n_len);
      ASSERT_TRUE(it);
      ASSERT_EQ(it, needle);
    }
    // printf("%s\n", it);
  }

  // printf("%s\n", text);

  delete[] text;
}
TEST(string_search, test_naive_large) {
  test_large(sp::naive::search);
}

TEST(string_search, test_boyer_more_large) {
  test_large(sp::bm::search);
}

static void
test_subset(test_search_fp search) {
  prng::xorshift32 r(1);
  // const std::size_t length = 25 * 1024;
  const std::size_t length = 40 * 100;

  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_xx);

  const char *it = text;
  const char *const end = it + length;
  for (; it != end && sp::distance(it, end) > 5; ++it) {
    const std::size_t n_len = sp::distance(it, end);
    // printf("[%zu]\n", n_len);
    const char *s = search(text, length, it, n_len);
    ASSERT_TRUE(s);
    ASSERT_EQ(s, it);
  }

  delete[] text;
}

TEST(string_search, test_boyer_more_subset) {
  test_subset(sp::bm::search);
}

TEST(string_search, test_naive_subset) {
  test_subset(sp::naive::search);
}

TEST(string_search, tsts) {
  const char *const text = ":|$2V7%CR'B(km^N5CB&|^Z@5frI@!nE";
  const char *const needle = "^N5CB&|^Z@5";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
}
