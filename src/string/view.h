#ifndef SP_UTIL_STRING_VIEW_H
#define SP_UTIL_STRING_VIEW_H

#include <cstddef>
#include <iostream>

namespace sp {
//=====================================
struct string_view {
  const char *str;
  std::size_t length;

  string_view(const char *, std::size_t) noexcept;
  explicit string_view(const char *) noexcept;

  bool
  operator==(const char *) const noexcept;

  bool
  operator==(const string_view &) const noexcept;

  explicit operator bool() const noexcept;
};

//=====================================
std::ostream &
operator<<(std::ostream &, const string_view &) noexcept;

} // namespace sp

#endif
