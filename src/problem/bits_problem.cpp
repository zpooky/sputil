#include "bits_problem.h"
#include <util/assert.h>

namespace prob {
//=====================================
static std::size_t
bits_count(unsigned char test) noexcept {
  std::size_t result = 0;
  const unsigned char mask = 1;

  for (std::size_t i = 0; i < 8; ++i) {
    if ((test & mask) == mask) {
      ++result;
    }
    test >>= 1;
  }

  return result;
}

std::size_t
bits_count(void *in, std::size_t length) noexcept {
  const unsigned char *const test = (unsigned char *)in;

  std::size_t result = 0;

  for (std::size_t i = 0; i < length; ++i) {
    assertx(test);
    result += bits_count(test[i]);
  }

  return result;
}

//=====================================
template <typename T>
static T
mask(std::size_t idx) noexcept {
  assertx(idx < (sizeof(T) * 8));
  T result = 1;
  return result << idx;
}

std::size_t
bits_reverse(std::size_t test) noexcept {

  std::size_t result = 0;
  const std::size_t bits(sizeof(test) * 8);
  std::size_t tail = bits - 1;

  for (std::size_t i = 0; i < bits; ++i) {
    std::size_t h = mask<std::size_t>(tail) & test;

    if (tail >= i) {
      h >>= (tail - i);
    } else {
      h <<= (i - tail);
    }
    result = result | h;
    tail--;
  }
  return result;
}

//=====================================
bool
bits_is_palindrome(std::size_t test) noexcept {
  const std::size_t bits = sizeof(test) * 8;
  std::size_t head = 0;
  std::size_t tail = (bits)-1;

  for (std::size_t i = 0; i < bits; ++i) {
    std::size_t h = mask<std::size_t>(head) & test;
    h >>= head++;

    std::size_t t = mask<std::size_t>(tail) & test;
    t >>= tail--;

    if (h != t) {
      // printf("fail:idx[%zu],f[%d],t[%d]\n", i, int(h), int(t));
      return false;
    }
  }

  return true;
}

//=====================================
}
