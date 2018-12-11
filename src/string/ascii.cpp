#include "ascii.h"
// #include <util/swap.h>
#include <util/array.h>
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
//=====================================
static void
reflect(char needle, std::size_t idx, char *str, std::size_t len) {
  if (idx < len) {
    bool swp = false;
    if (str[idx] == needle) {
      swp = true;
      std::size_t m = mirror(idx, len);
      // printf("swap(idx[%zu],m[%zu])\n", idx, m);
    }

    reflect(needle, idx + 1, str, len);

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
  rec::reflect(needle, 0, str, len);
}

//=====================================
bool
is_alpha(const char *str, std::size_t len) noexcept {
  for (std::size_t i = 0; i < len; ++i) {
    if (!isalpha(str[i])) {
      return false;
    }
  }
  return true;
}

//=====================================
bool
is_printable(char c) noexcept {
  return c >= ' ' && c <= '~';
}

bool
is_printable(const char *str, std::size_t len) noexcept {
  for (std::size_t i = 0; i < len; ++i) {
    if (!is_printable(str[i])) {
      return false;
    }
  }

  return true;
}

//=====================================
} // namespace ascii
