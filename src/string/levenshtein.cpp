#include "levenshtein.h"
#include <cstdio>
#include <cstring>
#include <util/Matrix.h>
#include <util/assert.h>

/*
 * the levenshtein distance between two words is the minimum number of
 * single-character edits:
 * - insertion of character
 * - delete of character
 * - substitute of charcter
 * required to change $first word into the other.
 */

namespace ascii {
//=====================================
static std::size_t
min(std::size_t f, std::size_t s) noexcept {
  return f < s ? f : s;
}

static std::size_t
min(std::size_t f, std::size_t s, std::size_t t) noexcept {
  return min(f, min(s, t));
}

//=====================================
static int xxxx = 0;
/* - Brute force
 * - divide and conquer
 * - problem: allot of redundant work
 */
std::size_t
levenshtein(const char *first, const char *second) noexcept {
  assertx(first);
  assertx(second);
  ++xxxx;

  if (*first == '\0') {
    return std::strlen(second);
  }

  if (*second == '\0') {
    return std::strlen(first);
  }

  if (*first == *second) {
    /* current chars are equal, cost = 0 */
    return levenshtein(first + 1, second + 1);
  }

  /*
   * Simulate that we inserted current char from $first into second.
   */
  auto ins = levenshtein(first + 1, second);
  /*
   * Simulate that we deleted currrent char from $second and retrying
   */
  auto del = levenshtein(first, second + 1);
  /*
   * Simulate that we replace current word from $second with current from $first
   */
  auto sub = levenshtein(first + 1, second + 1);

  return min(ins, del, sub) + 1;
}
void
p() {
  printf("%d\n", xxxx);
  xxxx = 0;
}

//=====================================
static std::size_t
levenshtein2(const char *s, size_t len_s, const char *t,
             size_t len_t) noexcept {
  ++xxxx;
  int cost;

  /* base case: empty strings */
  if (len_s == 0)
    return len_t;
  if (len_t == 0)
    return len_s;

  /* test if last characters of the strings match */
  if (s[len_s - 1] == t[len_t - 1])
    cost = 0;
  else
    cost = 1;

  /* return minimum of delete char from s, delete char from t, and delete char
   * from both */
  return min(levenshtein2(s, len_s - 1, t, len_t) + 1,
             levenshtein2(s, len_s, t, len_t - 1) + 1,
             levenshtein2(s, len_s - 1, t, len_t - 1) + cost);
}

std::size_t
levenshtein2(const char *s, const char *t) noexcept {
  return levenshtein2(s, std::strlen(s), t, std::strlen(t));
}

namespace dp {
//=====================================
/*
 *      S	a	t	u	r	d	a	y
 *    0	1	2	3	4	5	6	7	8
 * S	1	0	1	2	3	4	5	6	7
 * u	2	1	1	2	2	3	4	5	6
 * n	3	2	2	2	3	3	4	5	6
 * d	4	3	3	3	3	4	3	4	5
 * a	5	4	3	4	4	4	4	3	4
 * y	6	5	4	4	5	5	5	4	3
 */

/* y
 * ^
 * |
 * |
 * |______> x
 */
std::size_t
levenshtein(const char *first, const char *second) noexcept {
  // Implementation of Wagner–Fischer algorithm version of Levenshtein distance
  // https://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
  // - dynamic programming
  assertx(first);
  assertx(second);

  const std::size_t sec_len = std::strlen(second);
  if (*first == '\0') {
    return sec_len;
  }

  const std::size_t fst_len = std::strlen(first);
  if (*second == '\0') {
    return fst_len;
  }

  sp::Matrix<std::size_t> table(sec_len + 1 /*height*/, fst_len + 1 /*width*/);
  for (std::size_t x = 0; x < table.width; ++x) {
    // printf("table[0][x[%zu]] = x[%zu]\n", x, x);
    table[0][x] = x;
  }

  for (std::size_t y = 0; y < table.height; ++y) {
    // printf("table[y[%zu]][0] = y[%zu]\n", y, y);
    table[y][0] = y;
  }

  for (std::size_t y = 1; y < table.height; ++y) {
    char sec_current = second[y - 1];
    for (std::size_t x = 1; x < table.width; ++x) {
      char fst_current = first[x - 1];

      std::size_t indicator = fst_current == sec_current ? 0 : 1;

      std::size_t ins = table[y - 1][x] + 1;
      std::size_t del = table[y][x - 1] + 1;
      std::size_t sub = table[y - 1][x - 1] + indicator;

      table[y][x] = min(del, ins, sub);
    }
  }
  return table[table.height - 1][table.width - 1];
}

//=====================================
} // namespace dp
} // namespace ascii
