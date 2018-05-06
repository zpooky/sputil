#ifndef SP_UTIL_UTIL_COMPARATOR_H
#define SP_UTIL_UTIL_COMPARATOR_H

namespace sp {
struct less {
  template <typename T, typename K>
  constexpr bool
  operator()(const T &lhs, const K &rhs) const noexcept {
    return lhs < rhs;
  }
};

struct greater {
  template <typename T, typename K>
  constexpr
      /**/
      bool
      operator()(const T &lhs, const K &rhs) const noexcept {
    return lhs > rhs;
  }

  // template <typename T, typename K>
  // constexpr bool
  // operator()(const T *lhs, const K *rhs) const noexcept {
  //   return lhs > *rhs;
  // }
  //
  // template <typename T, typename K>
  // constexpr bool
  // operator()(const T *lhs, const K &rhs) const noexcept {
  //   return lhs > rhs;
  // }
  //
  // template <typename T, typename K>
  // constexpr bool
  // operator()(const T &lhs, const K *rhs) const noexcept {
  //   return lhs > *rhs;
  // }
};

struct present {
  template <typename T>
  constexpr bool
  operator()(const T &o) const noexcept {
    return bool(o);
  }
};

} // namespace sp

#endif
