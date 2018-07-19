#include "misc.h"
#include <collection/Array.h>
#include <cstdio>
#include <util/assert.h>

namespace sp {
std::size_t
represent(std::uint32_t N, const std::uint32_t *pool,
          std::size_t len) noexcept {
  if (N == 0) {
    return 1;
  }

  std::size_t result = 0;
  for (std::size_t i = 0; i < len; ++i) {
    auto current = pool[i];
    if (current <= N) {
      result += represent(N - current, pool, len);
    }
  }

  return result;
}

namespace dp {
static std::size_t
represent(DynamicArray<std::size_t> &memo, std::uint32_t N,
          const std::uint32_t *pool, std::size_t len) noexcept {
  if (N == 0) {
    memo[N] = 1;
  }

  if (memo[N] == 0) {
    std::size_t result = 0;
    for (std::size_t i = 0; i < len; ++i) {
      auto current = pool[i];
      if (current <= N) {
        result += represent(memo, N - current, pool, len);
      }
    }
    memo[N] = result;
    assertx(memo[N] == result);
  }

  return memo[N];
}

std::size_t
represent(std::uint32_t N, const std::uint32_t *pool,
          std::size_t len) noexcept {
  DynamicArray<std::size_t> memo(N + 1);
  memo.length = memo.capacity;

  assertxs(memo.length == N + 1, memo.length, N + 1);
  assertxs(memo.capacity == memo.length, memo.capacity, memo.length, N + 1);
  return represent(memo, N, pool, len);
}
}

//=====================================
std::size_t
dice_throw(const std::size_t N, const std::size_t dice,
           const std::size_t faces) noexcept {
  if (N == 0) {
    if (dice == 0) {
      return 1;
    }
  }
  if (dice == 0) {
    return 0;
  }

  std::size_t result = 0;

  for (std::size_t face = 1; face <= faces; ++face) {
    if (face <= N) {
      result += dice_throw(N - face, dice - 1, faces);
    }
  }

  return result;
}

// TODO dp https://www.geeksforgeeks.org/dice-throw-problem/
}
