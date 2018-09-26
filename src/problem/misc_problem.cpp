#include "misc_problem.h"
#include <collection/Array.h>
#include <cstdio>
#include <util/Bitset.h>
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

//=====================================
namespace impl {
static void
subsets(DynamicArray<std::uint32_t> &memo, std::uint32_t from, std::uint32_t to,
        u32_subset_cb cb, void *closure) noexcept {
  if (from == to) {
    cb(memo.data(), length(memo), closure);
  } else {
    subsets(memo, from + 1, to, cb, closure); // skipping

    std::uint32_t *pres = push(memo, from);
    assertx(pres);

    subsets(memo, from + 1, to, cb, closure); // including

    bool popres = pop(memo);
    assertx(popres);
  }
}
}

void
subsets(std::uint32_t from, std::uint32_t to, void *closure,
        u32_subset_cb cb) noexcept {
  assertxs(from <= to, from, to);
  DynamicArray<std::uint32_t> memo(to - from);
  impl::subsets(memo, from, to, cb, closure);
  assertxs(is_empty(memo), length(memo));
}

//=====================================
namespace impl {
static void
permutations(DynamicArray<std::uint32_t> &memo, Bitset &visited,
             const std::uint32_t from, const std::uint32_t to,
             u32_permutations_cb cb, void *closure) noexcept {
  if (is_full(memo)) {
    cb(memo.data(), length(memo), closure);
  } else {
    for (std::size_t i = from; i < to; ++i) {
      const std::size_t idx = i - from;

      // printf("i[%zu]. test(v,idx[%zu]): %s\n", i, idx,
      //        test(visited, idx) ? "true" : "false");
      if (test(visited, idx) == false) {
        bool priv = set(visited, idx, true);
        assertx(!priv);

        std::uint32_t *pres = push(memo, i);
        assertx(pres);

        permutations(memo, visited, from, to, cb, closure);

        bool popres = pop(memo);
        assertx(popres);

        priv = set(visited, idx, false);
        assertx(priv);
      }
    }
  }
}
}

void
permutations(std::uint32_t from, std::uint32_t to, void *closure,
             u32_permutations_cb cb) noexcept {
  assertxs(from <= to, from, to);
  DynamicArray<std::uint32_t> memo(to - from);

  DynamicBitset visited(Bitset_number_of_buffer(capacity(memo)));
  impl::permutations(memo, visited, from, to, cb, closure);

  assertxs(is_empty(memo), length(memo));
}

//=====================================
namespace impl {
static void
permutations_repeating(DynamicArray<std::uint32_t> &memo,
                       const std::uint32_t from, const std::uint32_t to,
                       u32_permutations_cb cb, void *closure) noexcept {
  if (is_full(memo)) {
    cb(memo.data(), length(memo), closure);
  } else {
    for (std::size_t i = from; i < to; ++i) {
      std::uint32_t *pres = push(memo, i);
      assertx(pres);

      permutations_repeating(memo, from, to, cb, closure);

      bool popres = pop(memo);
      assertx(popres);
    }
  }
}
}

void
permutations_repeating(std::uint32_t from, std::uint32_t to, void *closure,
                       u32_permutations_cb cb) noexcept {
  assertxs(from <= to, from, to);
  DynamicArray<std::uint32_t> memo(to - from);

  impl::permutations_repeating(memo, from, to, cb, closure);

  assertxs(is_empty(memo), length(memo));
}
}
