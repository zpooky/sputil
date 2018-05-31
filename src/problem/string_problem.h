#ifndef SP_UTIL_STRING_PROBLEM_H
#define SP_UTIL_STRING_PROBLEM_H

#include <cstddef>
#include <string/view.h>

namespace prob {
//=====================================
void
reverse(char *) noexcept;

namespace rec {
//=====================================
void
reverse(char *) noexcept;
}

//=====================================
bool
is_palindrome(const char *, std::size_t) noexcept;

bool
is_palindrome(const char *) noexcept;

//=====================================
namespace rec {
sp::string_view
longest_palindromic_substring(const char *, std::size_t);

sp::string_view
longest_palindromic_substring(const char *);
}
//=====================================
}

#endif
