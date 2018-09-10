#include "longest_palindromic_substring.h"
#include <problem/string_problem.h>
#include <util/assert.h>
#include <cstring>

#include <cstddef>

namespace np {
namespace rec {
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
}
