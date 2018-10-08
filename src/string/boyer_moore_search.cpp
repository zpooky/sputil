#include "boyer_moore_search.h"
#include <cstring>
#include <map/HashMapProbing.h>
#include <problem/string_problem.h>
#include <util/assert.h>

namespace sp {
namespace bm {
//=====================================
// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm

// https://www2.cs.duke.edu/courses/fall01/cps130/lectures/lect14/node18.html#SECTION00035000000000000000
static bool
last_occurrences(const char *n, std::size_t len,
                 sp::HashMapProbing<char, std::size_t> &res) noexcept {
  for (std::size_t i = 0; i < len; ++i) {
    assertx_n(insert(res, n[i], i));
  }

  return true;
}

static void
prefix(const char *n) noexcept {
}

// https://www2.cs.duke.edu/courses/fall01/cps130/lectures/lect14/node25.html#SECTION000312000000000000000
static bool
good_suffix(const char *n, std::size_t len) noexcept {
  prefix(n);
  return true;
}

#if 0
// https://www2.cs.duke.edu/courses/fall01/cps130/lectures/lect14/node15.html
const char *
search(const char *txt, std::size_t tlen /*n*/, const char *pattern,
       std::size_t plen /*m*/) noexcept {
  if (plen > tlen) {
    return nullptr;
  }

  /* Pre-process */
  sp::HashMapProbing<char, std::size_t> loccurence;
  last_occurrences(pattern, plen, loccurence);

  // good_suffix(pattern, plen);
  const std::size_t space = tlen - plen;
  std::size_t skip = 0;
  for (std::size_t i = 0; i <= space; i += skip) {
    skip = 0;

    for (std::size_t j = plen; j-- > 0;) {

      const std::size_t id = i + j;
      if (pattern[j] != txt[id]) {
        std::size_t *const l = lookup(loccurence, txt[id]);
        if (l) {
          skip = std::max<std::size_t>(1, j - *l);
        } else {
          skip = std::max<std::size_t>(1, j - -1);
        }
        break;
      }

    } // for

    if (skip == 0) {
      /* Found */
      return txt + i;
    }
  } // for

  return nullptr;
}
#endif

#if 0
/*
 * Compute function last
 * i ← m-1
 * j ← m-1
 * Repeat
 *     If P[j] = T[i] then
 *         if j=0 then
 *             return i        // we have a match
 *         else
 *             i ← i -1
 *             j ← j -1
 *     else
 *         i ← i + m - Min(j, 1 + last[T[i]])
 *         j ← m -1
 * until i > n -1
 * Return "no match"
 */
const char *
search(const char *txt, const std::size_t tlen /*n*/, const char *pattern,
       const std::size_t plen /*m*/) noexcept {
  if (plen > tlen) {
    return nullptr;
  }

  /* Pre-process */
  sp::HashMapProbing<char, std::size_t> loccurence;
  last_occurrences(pattern, plen, loccurence);

  std::size_t i = plen - 1;
  std::size_t j = plen - 1;
  do {
    if (pattern[j] == txt[i]) {
      if (j == 0) {
        return txt + i;
      }

      i = i - 1;
      j = j - 1;
    } else {
      const std::size_t *const l = lookup(loccurence, txt[i]);
      if (l) {
        i = (i + plen) - std::min<std::size_t>(j, 1 + *l);
      } else {
        i = (i + plen) - std::min<std::size_t>(j, 1 + -1);
      }
      j = plen - 1;
    }

  } while (i > (tlen - 1));

  return nullptr;
}

#endif

static bool
cmp_right_left(const char *txt, const char *pattern, std::size_t len) noexcept {
  /*
   * $txt points to the last character to compare while $pattern points to the
   * first.
   */
  const char *t = txt - (len - 1);
  return std::memcmp(t, pattern, len) == 0;
}

const char *
search(const char *const txt, const std::size_t tlen, //
       const char *const pattern, const std::size_t plen) noexcept {
  if (plen > tlen) {
    return nullptr;
  }

  /* Pre-process */
  sp::HashMapProbing<char, std::size_t> last;
  last_occurrences(pattern, plen, last);

  std::size_t i = plen - 1;
  while (i < tlen) {
    char current = txt[i];

    /* Compare from right to left */
    if (cmp_right_left(txt + i, pattern, plen)) {
      return txt + (i - (plen - 1));
    }

    std::size_t *l = lookup(last, current);
    if (l) {
      /* if $l is 0 then without +1 we will loop forever */
      i += (*l + 1);
    } else {
      /* current char is not in $last, skip ahead */
      i += plen;
    }
  }

  return nullptr;
}

const char *
search(const char *text, std::size_t tlen, const char *needle) noexcept {
  return search(text, tlen, needle, std::strlen(needle));
}

const char *
search(const char *text, const char *needle) noexcept {
  return search(text, std::strlen(text), needle);
}

//=====================================
}
}
