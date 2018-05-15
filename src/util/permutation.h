#ifndef SP_UTIL_UTIL_PERMUTATIONS_H
#define SP_UTIL_UTIL_PERMUTATIONS_H

#include <collection/Array.h>
#include <cstddef>
// #include <cstdio>
#include <util/assert.h>

namespace sp {
namespace rec {

//=====================================
/*
 * permutation of: [0,1]
 * with length 2:
 * - 01
 * - 00
 * - 11
 * - 10
 */
template <typename F>
bool
permutations(const char *pool, std::size_t length, F out) noexcept;

template <typename F>
bool
permutations(const char *pool, std::size_t length, std::size_t res_length,
             F out) noexcept;

std::size_t
number_of_permutations(std::size_t pool, std::size_t res) noexcept;

//=====================================
/*
 * combination of: [0,1]
 * - 01
 * - 10
 */
template <typename T, typename F, std::size_t n = 64>
bool
combinations(const T *pool, std::size_t length, F out) noexcept;

//=====================================
/*
 * optionals of: [0,1]
 * - 01
 * - 0
 * - 1
 * - ""
 */
template <typename T, typename F, std::size_t n = 64>
bool
optionals(const T *pool, std::size_t length, F out) noexcept;

//=====================================
//====Implementation===================
//=====================================
namespace impl {
template <typename F, std::size_t N>
static bool
permutations(const char *pool, std::size_t length, std::size_t idx,
             std::size_t result_length, char (&buffer)[N], F out) noexcept {
  if (idx < result_length) {
    for (std::size_t i = 0; i < length; ++i) {
      buffer[idx] = pool[i];
      if (!permutations(pool, length, idx + 1, result_length, buffer, out)) {
        return false;
      }
    }
  } else {
    return out(buffer);
  }

  return true;
}
}

template <typename F>
bool
permutations(const char *pool, std::size_t length, F out) noexcept {
  std::size_t res_length = length;
  return permutations(pool, length, res_length, out);
}

template <typename F>
bool
permutations(const char *pool, std::size_t length, std::size_t res_length,
             F out) noexcept {

  char buffer[50] = {0};
  return impl::permutations(pool, length, 0, res_length, buffer, out);
}

//=====================================
namespace impl {
template <typename T, typename F, std::size_t len>
static bool
combinations(const sp::StaticArray<const T *, len> &pool,
             sp::StaticArray<const T *, len> &result, std::size_t idx,
             F out) noexcept {
  if (!is_empty(pool)) {
    for (std::size_t i = 0; i < length(pool); ++i) {
      sp::StaticArray<const T *, len> sub_pool;
      {
        assertx(insert_all(sub_pool, pool));
        assertx(remove(sub_pool, i));
      }

      assertx(set(result, idx, pool[i]));
      if (!combinations(sub_pool, result, idx + 1, out)) {
        return false;
      }
    }

  } else {
    return out(result);
  }
  return true;
}
} // namespace impl

template <typename T, typename F, std::size_t len>
bool
combinations(const T *pool, std::size_t l, F out) noexcept {
  sp::StaticArray<const T *, len> apool;
  for (std::size_t i = 0; i < l; ++i) {
    assertx(insert(apool, pool + i));
  }

  sp::StaticArray<const T *, len> result;
  return impl::combinations(apool, result, 0, out);
}

//=====================================
namespace impl {
template <typename T, typename F, std::size_t len>
static bool
optionals(const sp::StaticArray<const T *, len> &pool, std::size_t pool_idx,
          sp::StaticArray<const T *, len> &result, std::size_t idx,
          F out) noexcept {
  if (pool_idx < pool.length) {
    /*some*/ {
      assertx(set(result, idx, pool[pool_idx]));
      if (!optionals(pool, pool_idx + 1, result, idx + 1, out)) {
        return false;
      }
    }
    /*none*/ {
      if (!optionals(pool, pool_idx + 1, result, idx, out)) {
        return false;
      }
    }

  } else {
    const std::size_t blength = length(result);
    result.length = idx;
    bool res = out(result);
    result.length = blength;
    return res;
  }

  return true;
}
}

template <typename T, typename F, std::size_t len>
bool
optionals(const T *pool, std::size_t length, F out) noexcept {
  sp::StaticArray<const T *, len> apool;
  for (std::size_t i = 0; i < length; ++i) {
    assertx(insert(apool, pool + i));
  }

  sp::StaticArray<const T *, len> result;
  result.length = result.capacity;

  return impl::optionals(apool, 0, result, 0, out);
}

//=====================================
} // namespace sp::rec
} // namespace sp

#endif
