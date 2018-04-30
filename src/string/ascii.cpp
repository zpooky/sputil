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

}
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
}
