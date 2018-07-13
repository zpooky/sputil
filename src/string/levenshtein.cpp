#include "levenshtein.h"
#include <util/assert.h>

/*
 * the Levenshtein distance between two words is the minimum number of
 * single-character edits:
 * - insertion character
 * - delete character
 * - substitute charcter
 * required to change one word into the other.
 */

namespace ascii {

static std::uint32_t
min(std::uint32_t f, std::uint32_t s) noexcept {
  return f < s ? f : s;
}

static std::uint32_t
min(std::uint32_t f, std::uint32_t s, std::uint32_t t) noexcept {
  return min(f, min(s, t));
}

std::uint32_t
levenshtein(const char *first, const char *second) noexcept {
  assertx(first);
  assertx(second);

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
   * Simulate that we edit current word from $second with current from $first
   */
  auto sub = levenshtein(first + 1, second + 1);

  return min(ins, del, sub) + 1;
}
}
