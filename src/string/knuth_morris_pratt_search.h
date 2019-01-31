#ifndef SP_UTIL_STRING_KNUTH_MORRIS_PRATT_SEARCH_H
#define SP_UTIL_STRING_KNUTH_MORRIS_PRATT_SEARCH_H

#include <cstddef>
// https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm

// TODO document
namespace sp {
namespace kmp {
//=====================================
const char *
search(const char *text, std::size_t, const char *needle, std::size_t) noexcept;

const char *
search(const char *text, std::size_t, const char *needle) noexcept;

const char *
search(const char *text, const char *needle) noexcept;

//=====================================
} // namespace kmp
} // namespace sp

#endif
