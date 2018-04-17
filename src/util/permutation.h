#ifndef SP_UTIL_PERMUTATIONS_H
#define SP_UTIL_PERMUTATIONS_H

#include <collection/Array.h>
#include <cstddef>
#include <cstdio>

namespace sp {
namespace rec {

//=====================================
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

template <typename T, typename F, std::size_t n = 64>
bool
combinations(const T *pool, std::size_t length, F out) noexcept;

//=====================================

/*
 * =======================================================
 */

//=====================================
namespace impl {
template <typename F, std::size_t N>
bool
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
    out(buffer);
  }
  return true;
}

template <typename T, typename F, std::size_t len>
bool
combinations(const sp::StaticArray<const T *, len> &pool,
             sp::StaticArray<const T *, len> &result, std::size_t idx, bool opt,
             F out) noexcept {
  if (!is_empty(pool)) {
    for (std::size_t i = 0; i < length(pool); ++i) {
      assert(set(result, idx, pool[i]));

      sp::StaticArray<const T *, len> sub_pool;
      {
        assert(insert_all(sub_pool, pool));
        assert(remove(sub_pool, i));
      }

      if (!combinations(sub_pool, result, idx + 1, opt, out)) {
        return false;
      }
    }
  } else {
    out(result);
  }
  return true;
}

} // namespace impl

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
// TODO combinations with support for skipping current level and continue to the
// next
template <typename T, typename F, std::size_t len>
bool
combinations(const T *pool, std::size_t length, F out) noexcept {
  sp::StaticArray<const T *, len> apool;
  for (std::size_t i = 0; i < length; ++i) {
    assert(insert(apool, pool + i));
  }
  sp::StaticArray<const T *, len> result;

  return impl::combinations(apool, result, 0, false, out);
}

} // namespace sp::rec
} // namespace sp

#endif
