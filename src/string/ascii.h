#ifndef SP_UTIL_STRING_UTIL_H
#define SP_UTIL_STRING_UTIL_H

#include <string/view.h>
#include <cstddef>

namespace ascii {
//=====================================

void
reflect(char needle, char *str, std::size_t length) noexcept;
//=====================================

// Given a string with distinct lowercase alphabetical letters such as abcdefg,
// define a reflection of a letter in the string c as moving it (without
// changing the order of any other letter) to a new place in the string
// such that the number of letters originally to the right of it is now the
// number of letters to the left of it.

void
reflect_right(char needle, char *str, std::size_t length) noexcept;

//=====================================

void
reverse(char *str, std::size_t length) noexcept;

//=====================================

sp::string_view
longest_palindromic_substring(const char *str, std::size_t);

//=====================================
} // namespace ascii

#endif
