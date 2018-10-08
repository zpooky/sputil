#include "gtest/gtest.h"
#include <cstring>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <string/ascii.h>

template <std::size_t N>
static bool
cmp_reverse(char (&straight)[N], char (&backwards)[N], std::size_t len) {
  if (len > 0) {
    char *first = straight;
    char *last = backwards + (len - 1);
    for (std::size_t i = 0; i < len; ++i) {
      if (*first++ != *last--) {
        return false;
      }
    }
  }
  return true;
}

TEST(StringUtil, test_reflect) {
  {
    char str[] = "reverse";
    ascii::reflect('r', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "eerevsr") == 0);
  }
  // printf("===\n");
  {
    char str[] = "reverse";
    ascii::reflect('e', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "esverer") == 0);
  }
  // printf("===\n");
  {
    char str[] = "reverse";
    ascii::reflect('v', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "rerevse") == 0);
  }
  // printf("===\n");
  {
    char str[] = "reverse";
    ascii::reflect('s', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "rsveree") == 0);
  }
}

TEST(StringUtil, test_reflect2) {
  {
    char str[] = "a";
    ascii::reflect('a', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "a") == 0);
  }
  {
    char str[] = "a";
    ascii::reflect('v', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "a") == 0);
  }
  {
    char str[] = "ab";
    ascii::reflect('a', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "ba") == 0);
  }
  {
    char str[] = "ab";
    ascii::reflect('b', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "ba") == 0);
  }
  {
    char str[] = "aa";
    ascii::reflect('a', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "aa") == 0);
  }
  {
    char str[] = "ab";
    ascii::reflect('v', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "ab") == 0);
  }
  {
    char str[] = "aba";
    ascii::reflect('a', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "aba") == 0);
  }
  {
    char str[] = "aba";
    ascii::reflect('b', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "aba") == 0);
  }
  {
    char str[] = "myfriend";
    ascii::reflect('e', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "myerifnd") == 0);
  }
  {
    char str[] = "axyz";
    ascii::reflect('a', str, std::strlen(str));
    // printf("%s\n", str);
    ASSERT_TRUE(std::strcmp(str, "zxya") == 0);
  }
}

TEST(StringUtil, test_right_reflect) {
}

