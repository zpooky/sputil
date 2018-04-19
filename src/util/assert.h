#ifndef SP_UTIL_UTIL_ASSERT_H
#define SP_UTIL_UTIL_ASSERT_H

namespace sp {
namespace impl {

[[noreturn]] void
assert_func(const char *, int, const char *, const char *) noexcept;

} // namespace sp::impl
} // namespace sp

#ifdef NDEBUG
// Note: here the expression gets nooped and will never be executed
#define assertx(__e) ((void)0)
#else
#define assertx(__e)                                                           \
  ((__e) ? (void)0 : sp::impl::assert_func(__FILE__, __LINE__, "", #__e))
#endif

#endif
