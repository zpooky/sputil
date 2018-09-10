#include "string_problem.h"
#include <cstring>
#include <util/assert.h>
#include <utility>

namespace prob {
//=====================================
void
reverse(char *const str) noexcept {
  if (!str)
    return;
  const std::size_t length = std::strlen(str);
  if (length <= 1)
    return;

  char *head_it = str;
  char *tail_it = str + length - 1;
  while (head_it < tail_it) {
    using std::swap;
    swap(*head_it++, *tail_it--);
  }
}

//=====================================
namespace rec {
namespace impl {
char *
reverse(char *start, char *str) noexcept {
  const char c = *str;
  if (c != '\0') {
    char *const ins = reverse(start, str + 1);
    *ins = c;
    return ins + 1;
  }

  return start;
}
}

void
reverse(char *const str) noexcept {
  impl::reverse(str, str);
}
}

//=====================================
bool
is_palindrome(const char *str, std::size_t length) noexcept {
  if (length <= 1) {
    return true;
  }
  assertx(str);

  const char *head_it = str;
  const char *tail_it = str + length - 1;
  while (head_it < tail_it) {
    if (*head_it++ != *tail_it--) {
      return false;
    }
  }

  return true;
}

bool
is_palindrome(const char *str) noexcept {
  return is_palindrome(str, std::strlen(str));
}

}
