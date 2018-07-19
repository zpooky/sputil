#ifndef SP_UTIL_BITS_PROBLEM_H
#define SP_UTIL_BITS_PROBLEM_H

#include <cstddef>

namespace prob {
//=====================================
std::size_t
bits_count(void *, std::size_t) noexcept;

//=====================================
std::size_t bits_reverse(std::size_t) noexcept;

//=====================================
bool bits_is_palindrome(std::size_t) noexcept;

//=====================================
}

#endif
