#include "longest_palindromic_substring.h"
#include <cstring>
#include <problem/string_problem.h>
#include <util/assert.h>

#include <cstddef>

namespace np {
namespace rec {
//=====================================
sp::string_view
longest_palindromic_substring(const char *const str, const std::size_t length) {
  if (length == 0) {
    return {nullptr, 0};
  }

  std::cout << sp::string_view(str, length) << "\n";
  if (prob::is_palindrome(str, length)) {
    return {str, length};
  }
  assertx(length > 1);
  assertx(length < 64);
  // printf("length[%zu]\n", length);

  auto left = longest_palindromic_substring(str, length - 1);
  if (left.length == length - 1) {
    return left;
  }

  auto right = longest_palindromic_substring(str + 1, length - 1);
  if (left.length >= right.length) {
    return left;
  }

  return right;
}

sp::string_view
longest_palindromic_substring(const char *str) {
  return longest_palindromic_substring(str, std::strlen(str));
}
}

//=====================================
sp::string_view
longest_palindromic_substring(const char *str, std::size_t str_length) {
  const char *res = nullptr;
  std::size_t res_len = 0;

  for (std::size_t strt = 0; strt < str_length; ++strt) {
    for (std::size_t len = std::strlen(str + strt) + 1; len-- > 0;) {

      if (prob::is_palindrome(str + strt, len)) {
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
}
