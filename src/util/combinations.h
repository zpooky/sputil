#ifndef SP_UTIL_UTIL_COMBINATIONS_H
#define SP_UTIL_UTIL_COMBINATIONS_H

#include <collection/Array.h>
#include <cstddef>
// #include <cstdio>
#include <type_traits>
#include <util/assert.h>

namespace sp {
namespace rec {
//=====================================
/*
 * combination of: [0,1]
 * - 01
 * - 10
 */
template <typename T, typename F, std::size_t n = 64>
bool
combinations(const T *pool, std::size_t length, F visit) noexcept;

std::size_t combinations_of(std::size_t) noexcept;

//=====================================
namespace impl {
template <typename T, typename F, std::size_t len>
static bool
combinations(const sp::StaticArray<const T *, len> &pool,
             sp::StaticArray<const T *, len> &result, std::size_t idx,
             F visit) noexcept {
  if (is_empty(pool)) {
    return visit(result);
  }

  for (std::size_t i = 0; i < length(pool); ++i) {
    sp::StaticArray<const T *, len> sub_pool;
    {
      bool res = insert_all(sub_pool, pool);
      assertx(res);

      res = remove(sub_pool, i);
      assertx(res);
    }

    bool res = set(result, idx, pool[i]);
    assertx(res);
    if (!combinations(sub_pool, result, idx + 1, visit)) {
      return false;
    }
  }

  return true;
}
} // namespace impl

template <typename T, typename F, std::size_t len>
bool
combinations(const T *pool, std::size_t l, F visit) noexcept {
  sp::StaticArray<const T *, len> apool;
  for (std::size_t i = 0; i < l; ++i) {
    bool res = insert(apool, pool + i);
    assertx(res);
  }

  sp::StaticArray<const T *, len> result;
  return impl::combinations(apool, result, 0, visit);
}
}
}

#endif
