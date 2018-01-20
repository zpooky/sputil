#ifndef SP_UTIL_UTIL_COMPARATOR_H
#define SP_UTIL_UTIL_COMPARATOR_H

namespace sp {
struct less {
  template <typename T>
  constexpr bool
  operator()(const T &lhs, const T &rhs) const {
    return lhs < rhs;
  }
};

struct greater {
  template <typename T>
  constexpr bool
  operator()(const T &lhs, const T &rhs) const {
    return lhs > rhs;
  }
};

struct present {
  template <typename T>
  constexpr bool
  operator()(const T &o) const {
    return bool(o);
  }
};

} // namespace sp

#endif
