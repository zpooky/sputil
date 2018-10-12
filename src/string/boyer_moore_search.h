#ifndef SP_UTIL_STRING_KNUTH_MORRIS_PRATT_SEARCH_H
#define SP_UTIL_STRING_KNUTH_MORRIS_PRATT_SEARCH_H

#include <cstddef>

// good:
// https://people.ok.ubc.ca/ylucet/DS/BoyerMoore.html
//
// ref:
// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm
// https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/BoyerMoore.java.html
namespace sp {
namespace bm {
//=====================================
const char *
search(const char *text, std::size_t, const char *needle, std::size_t) noexcept;

const char *
search(const char *text, std::size_t, const char *needle) noexcept;

const char *
search(const char *text, const char *needle) noexcept;

//=====================================
}
}

#endif
