#ifndef SP_UTIL_UTIL_ASSERT_H
#define SP_UTIL_UTIL_ASSERT_H

#ifdef NDEBUG
#else
#include <cstring>
#include <iostream>
// #include <algorithm>
#endif

namespace sp {
namespace impl {

[[noreturn]] void
assert_func(const char *, int, const char *, const char *) noexcept;

} // namespace sp::impl
} // namespace sp

#ifdef NDEBUG
// Note: here the expression gets nooped and will never be executed
#define assertx(...) ((void)0)
#define assertx(__e) ((void)0)

#else

namespace sp {
namespace impl {

// https://stackoverflow.com/questions/32226300/make-variadic-macro-method-which-prints-all-variables-names-and-values
template <typename H1>
std::ostream &
show(std::ostream &out, const char *label, H1 &&value) {
  return out << label << "=" << std::forward<H1>(value) << '\n';
}

template <typename H1, typename... T>
std::ostream &
show(std::ostream &out, const char *label, H1 &&value, T &&... rest) {
  const char *pcomma = std::strchr(label, ',');
  return show(out.write(label, pcomma - label)
                  << "=" << std::forward<H1>(value) << ',',
              pcomma + 1, std::forward<T>(rest)...);
}
}
}

#define assertxs(__e, ...)                                                     \
  do {                                                                         \
    if (!(__e)) {                                                              \
      printf("\n");                                                            \
      sp::impl::show(std::cout, #__VA_ARGS__, __VA_ARGS__);                    \
      sp::impl::assert_func(__FILE__, __LINE__, "", #__e);                     \
    }                                                                          \
  } while (0)

#define assertx(__e)                                                           \
  do {                                                                         \
    if (!(__e)) {                                                              \
      printf("\n");                                                            \
      sp::impl::assert_func(__FILE__, __LINE__, "", #__e);                     \
    }                                                                          \
  } while (0)
#endif

#endif
