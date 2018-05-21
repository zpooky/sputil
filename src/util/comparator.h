#ifndef SP_UTIL_UTIL_COMPARATOR_H
#define SP_UTIL_UTIL_COMPARATOR_H

// TODO namespace cmp = cmp::greater?
namespace sp {
/*Ascending order: 0,1,2,3*/
struct less {
  template <typename T, typename K>
  constexpr bool
  operator()(const T &lhs, const K &rhs) const noexcept {
    return lhs < rhs;
  }
};

/*Descending order: 3,2,1,0*/
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

template <typename Cmp>
struct inverse {
  template <typename T, typename K>
  constexpr bool
  operator()(const T &lhs, const K &rhs) const noexcept {
    Cmp cmp;
    return cmp(rhs, lhs);
  }
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
