#include "ascii.h"
// #include <util/swap.h>
#include <util/assert.h>
#include <utility>

#include <cstdio>
#include <cstring>

namespace ascii {

//=====================================

static std::size_t
find_char(char needle, std::size_t idx, const char *str,
          std::size_t len) noexcept {
  for (std::size_t i = idx; i < len; ++i) {
    if (str[i] == needle) {
      return i;
    }
  }
  return len;
}

static std::size_t
mirror(std::size_t idx, std::size_t len) noexcept {
  std::size_t last = len - 1;
  std::size_t m = last - idx;
  return m;
}

namespace rec {
void
redirect(char needle, std::size_t idx, char *str, std::size_t len) {
  if (idx < len) {
    bool swp = false;
    if (str[idx] == needle) {
      swp = true;
      std::size_t m = mirror(idx, len);
      // printf("swap(idx[%zu],m[%zu])\n", idx, m);
    }

    redirect(needle, idx + 1, str, len);

    if (swp) {
      std::size_t m = mirror(idx, len);
      using std::swap;
      if (m != idx)
        swap(str[idx], str[m]);
    }
  }
}

} // namespace rec
void
reflect(char needle, char *str, std::size_t len) noexcept {
  rec::redirect(needle, 0, str, len);
}
//=====================================
void
reflect_right(char needle, char *str, std::size_t length) noexcept {
}

//=====================================

void
reverse(char *str, std::size_t length) noexcept {
  if (str) {
    if (length > 0) {
      // printf("strlen: %zu, length: %zu\n", std::strlen(str), length);
      // assertx(std::strlen(str) == length);

      char *tail = str + (length - 1);
      char *head = str;
      while (head < tail) {
        using std::swap;
        swap(*head, *tail);
        --tail;
        ++head;
      }
    }
  }
}
//=====================================
static bool
is_palindrome(const char *str, std::size_t length) {
  // printf("is_palindrome(%.*s,%zu): ", length, str, length);
  if (length > 0) {
    const char *head = str;
    const char *tail = str + (length - 1);
    while (head < tail) {
      if (*head++ != *tail--) {
        // printf("false\n");
        return false;
      }
    }
  }
  // printf("true\n");
  return true;
  // return false;
}
#if 0
namespace rec {

static std::string
longest_palindromic_substring(const char *p, const char *str, const char *end) {
  if (str == end) {
    return std::string(p, str - p);
  }

  if (!is_palindrome(p, str - p)) {
    ++p;
  }

  auto res = longest_palindromic_substring(p, str + 1, end);
  if (res.length() > str - p) {
    return res;
  }
  return std::string(p, str - p);
}

} // namespace rec
#endif

//TODO make better
sp::string_view
longest_palindromic_substring(const char *str, std::size_t str_length) {
  const char *res = nullptr;
  std::size_t res_len = 0;

  for (std::size_t strt = 0; strt < str_length; ++strt) {
    for (std::size_t len = std::strlen(str + strt) + 1; len-- > 0;) {

      if (is_palindrome(str + strt, len)) {
        if (len > res_len) {
          res = str + strt;
          res_len = len;
        }
        break;
      }
    }
  }

  return sp::string_view(res, res_len);
}

//=====================================
} // namespace ascii
