#ifndef SP_UTIL_SORT_UTIL_H
#define SP_UTIL_SORT_UTIL_H

#include <cstddef>
#include <util/comparator.h>

/*
 * # Decending
 * int numbers[]={20,40,50,10,30};
 * std::sort (numbers, numbers+5, std::greater<int>());
 * out: 50 40 30 20 10
 *
 * # Ascending
 * int foo[]={10,20,5,15,25};
 * std::sort (foo, foo+5, std::less<int>());
 * out: 5 10 15 20 25
 */
namespace sp {
//=====================================
template <typename T, typename Comparator = sp::less>
bool
is_sorted(T *, std::size_t) noexcept;

//=====================================
template <typename T, typename Comparator>
bool
is_sorted(T *const in, std::size_t length) noexcept {
  Comparator cmp;

  const T *it = in;
  const T *const end = in + length;
  while (it != end) {
    const T *const next = it + 1;
    if (next != end) {
      Comparator cmp;
      if (!cmp(*it, *next)) {
        return false;
      }
    }
    ++it;
  }

  return true;
}

//=====================================
}

#endif
