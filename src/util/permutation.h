#ifndef SP_UTIL_UTIL_PERMUTATIONS_H
#define SP_UTIL_UTIL_PERMUTATIONS_H

#include <collection/Array.h>
#include <cstddef>
// #include <cstdio>
#include <type_traits>
#include <util/assert.h>

namespace sp {
namespace rec {
/*
 * Recursion depth depends on result length
 *
 * TODO working array dynamic size determination
 */
//=====================================
/*
 * repeating permutation of: [0,1], length 2:
 * - 01
 * - 00
 * - 11
 * - 10
 */
template <typename F>
bool
permutations(const char *pool, std::size_t length, std::size_t res_length,
             F out) noexcept;

template <typename F>
bool
permutations(const char *pool, std::size_t length, F out) noexcept;

template <typename T, typename F, std::size_t n = 64>
bool
permutations(const T *pool, std::size_t length, std::size_t res_length,
             F out) noexcept;

template <typename T, typename F, std::size_t n = 64>
bool
permutations(const T *pool, std::size_t length, F out) noexcept;

std::size_t
permutations_of(std::size_t pool, std::size_t res_length) noexcept;

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

template <typename T, typename F>
static bool
permutations(const sp::Array<const T> &pool, sp::Array<const T *> &result,
             std::size_t result_length, std::size_t current, F out) noexcept {
  if (current < result_length) {
    for (std::size_t i = 0; i < length(pool); ++i) {
      result[current] = &pool[i];

      if (!permutations(pool, result, result_length, current + 1, out)) {
        return false;
      }
    }
  } else {
    return out(result);
  }

  return true;
}
} // namespace impl

template <typename F>
bool
permutations(const char *pool, std::size_t length, std::size_t res_length,
             F out) noexcept {
  char buffer[50] = {0};
  assertxs(res_length < sizeof(buffer), res_length, sizeof(buffer));

  return impl::permutations(pool, length, 0, res_length, buffer, out);
}

template <typename F>
bool
permutations(const char *pool, std::size_t length, F out) noexcept {
  std::size_t res_length = length;
  return permutations(pool, length, res_length, out);
}

template <typename T, typename F, std::size_t n>
bool
permutations(const T *pool, std::size_t length, std::size_t res_length,
             F out) noexcept {
  assertxs(res_length < n, res_length, n);

  using TT = typename std::remove_pointer<T>::type;
  sp::Array<const TT> apool(pool, length, length);

  sp::StaticArray<const T *, n> result;
  result.length = res_length;

  return impl::permutations(apool, result, res_length, 0, out);
}

template <typename T, typename F, std::size_t n>
bool
permutations(const T *pool, std::size_t length, F out) noexcept {
  return permutations<T, F, n>(pool, length, length, out);
}

} // namespace rec
} // namespace sp

#endif
