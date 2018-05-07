#ifndef SP_UTIL_MAP_SET_H
#define SP_UTIL_MAP_SET_H

#include <hash/util.h>

namespace sp {
template <typename T, sp::Hasher<T>>
struct Set {
  /**/
};
//=====================================
template <typename T, sp::Hasher<T> h, typename V>
T *
insert(Set<T, h> &, V &&) noexcept;
//=====================================
template <typename T, sp::Hasher<T> h, typename V>
bool
contains(const Set<T, h> &, const V &) noexcept;
//=====================================
//=====================================

template <typename T, sp::Hasher<T> h, typename V>
T *
insert(Set<T, h> &, V &&) noexcept {
  // TODO
  return nullptr;
}
}

#endif
