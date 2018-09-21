#ifndef SP_UTIL_UTIL_SUBSETS_H
#define SP_UTIL_UTIL_SUBSETS_H

#include <collection/Array.h>
#include <cstddef>
// #include <cstdio>
#include <type_traits>
#include <util/assert.h>

namespace sp {
namespace rec {
//=====================================
/*
 * subsets of: [0, 1]
 * - 0, 1
 * - 0
 * - 1
 * - ""
 */
template <typename T, typename F, std::size_t n = 64>
bool
subsets(const T *pool, std::size_t length, F out) noexcept;

//=====================================
namespace impl {
template <typename T, typename F, std::size_t len>
static bool
subsets(const sp::StaticArray<const T *, len> &pool, std::size_t pool_idx,
        sp::StaticArray<const T *, len> &result, std::size_t idx,
        F visit) noexcept {
  // printf("pool_idx[%zu] < pool.length[%zu]\n", pool_idx, pool.length);
  // printf("pool[len[%zu],cap[%zu],raw[%p]]\n", pool.length, pool.capacity,
  //        pool.buffer);
  // printf("result[len[%zu],cap[%zu],raw[%p]]\n", result.length, result.capacity,
  //        result.buffer);
  // printf("---------\n");
  if (pool_idx < pool.length) {
    /*include pool[pool_idx]*/ {
      assertx_n(set(result, idx, pool[pool_idx]));

      if (!subsets(pool, pool_idx + 1, result, idx + 1, visit)) {
        return false;
      }
    }
    /*skip pool[pool_idx]*/ {
      if (!subsets(pool, pool_idx + 1, result, idx, visit)) {
        return false;
      }
    }

  } else {
    const std::size_t blength = length(result);
    result.length = idx;
    bool res = visit(result);
    result.length = blength;

    return res;
  }

  return true;
}

} // namespace impl

template <typename T, typename F, std::size_t len>
bool
subsets(const T *const pool, std::size_t length, F out) noexcept {
  sp::StaticArray<const T *, len> apool;
  for (std::size_t i = 0; i < length; ++i) {
    assertx_n(insert(apool, pool + i));
  }

  sp::StaticArray<const T *, len> result;
  result.length = result.capacity;

  return impl::subsets(apool, 0, result, 0, out);
}
}
}

#endif
