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
        // is equal
        return txt + i;
      }

      --i ;
      --j ;
    } else {
      const std::size_t *const l = lookup(loccurence, txt[i]);
      if (l) {
        i = (i + plen) - std::min<std::size_t>(j, 1 + *l);
      } else {
        i = (i + plen) - std::min<std::size_t>(j, 1 + -1);
      }
      //reset
      j = plen - 1;
    }

  } while (i > (tlen - 1));

  return nullptr;
}

#endif

static const char *
compare_reverse(const char *txt, const char *pat, std::size_t &len) noexcept {
  /* $txt points to the last character to compare while $pattern points to the
   * first.
   */
  if (len == 0) {
    return nullptr;
  }

  // printf("*txt[%c] == *pat[%c]: ", *txt, *pat);
  if (*txt != *pat) {
    // printf("false\n");
    return txt;
  }
  // printf("true\n");

  --len;

  return compare_reverse(txt - 1, pat - 1, len);
}

/*
 * txt: | 0 | 1 | 2 | a | b | c | d | e | f |
 *
 * 1.    |   |   |   |
 *
 * 2.                |   |   |   |
 */

const char *
search(const char *const txt, const std::size_t tlen, //
       const char *const pattern, const std::size_t plen) noexcept {
  if (plen > tlen) {
    return nullptr;
  }

  /* Pre-process */
  sp::HashMapProbing<char, std::size_t> lstoc;
  last_occurrences(pattern, plen, lstoc);

  std::size_t i = plen - 1;
  while (i < tlen) {
    char current = txt[i];

    /* Compare from right to left */
    const char *const pat_last = pattern + (plen - 1);
    std::size_t out_len = plen;
    const char *const last = compare_reverse(txt + i, pat_last, out_len);
    if (!last) {
      return txt + (i - (plen - 1));
    }

    const std::size_t *l = lookup(lstoc, *last);
    if (l) {
      assertxs(out_len > 0, out_len);
      out_len--;

// printf("last[%c] = %zu, skip = ", *last, *l);
/* Here we should place $pattern[last($last)] in the same column as
 * $txt[$i].
 */

#if 0
      if (out_len <= *l) {
        printf("out_len[%zu], *l[%zu]\n", out_len, *l);

        return nullptr; // TODO
        printf("txt[%.*s]:%zu\n", tlen, txt, tlen);
        printf("ptn[%.*s]:%zu\n", plen, pattern, plen);
        assertxs(out_len > *l, out_len, *l);
      }
      const auto advance3 = out_len - *l;
#endif
      const auto advance3 = out_len > *l ? out_len - *l : *l - out_len;

      const auto advance = plen - *l;
      assertxs(advance > 0, advance);

      const std::size_t before_i = i;

      // printf("%zu\n", advance3);
      i = (i) + advance3;

      assertxs(plen >= advance3, plen, advance3);
      assertxs(pattern[plen - advance] == *last, pattern[plen - advance], *last,
               plen, advance, (plen - advance));

      if (i == before_i) {
        printf("txt[%.*s]:%zu\n", tlen, txt, tlen);
        printf("ptn[%.*s]:%zu\n", plen, pattern, plen);
        // i += plen;
        assertxs(i != before_i, i, before_i);
      }
    } else {
      // printf("last[%c] = -1, skip = ", *last);
      assertxs(out_len > 0, i, out_len);
      /* current char is not in $last, skip ahead */
      // printf("%zu\n", out_len);
      i += out_len;
    }
  }

  return nullptr;
}

const char *
search(const char *const txt, std::size_t tlen, const char *pattern) noexcept {
  return search(txt, tlen, pattern, std::strlen(pattern));
}

const char *
search(const char *const txt, const char *const pattern) noexcept {
  return search(txt, std::strlen(txt), pattern);
}

//=====================================
}
}
