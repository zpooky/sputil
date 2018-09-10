#ifndef SP_UTIL_PROBLEM_STRING_PROBLEM_H
#define SP_UTIL_PROBLEM_STRING_PROBLEM_H

#include <cstddef>

namespace prob {
//=====================================
void
reverse(char *) noexcept;

namespace rec {
void
reverse(char *) noexcept;
}

//=====================================
bool
is_palindrome(const char *, std::size_t) noexcept;

bool
is_palindrome(const char *) noexcept;
}

#endif
