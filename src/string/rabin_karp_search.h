#ifndef SP_UTIL_STRING_RABIN_KARP_SEARCH_H
#define SP_UTIL_STRING_RABIN_KARP_SEARCH_H

#include <cstddef>
/* # Rabin–Karp
 * ## ref
 * https://www.infoarena.ro/blog/rolling-hash
 * http://courses.csail.mit.edu/6.006/spring11/rec/rec06.pdf
 * https://www.quora.com/What-is-a-rolling-hash-and-when-is-it-useful
 * https://brilliant.org/wiki/rabin-karp-algorithm/
 * https://github.com/lemire/rollinghashcpp/blob/master/rabinkarphash.h
 * https://www.baeldung.com/java-full-text-search-algorithms
 * https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm
 *
 * #
 * The Rabin–Karp algorithm is inferior for single pattern searching to
 * Knuth–Morris–Pratt algorithm, Boyer–Moore string search algorithm and other
 * faster single pattern string searching algorithms because of its slow worst
 * case behavior. However, it is an algorithm of choice for multiple pattern
 * search
 *
 * That is, if we want to find any of a large number, say k, fixed length
 * patterns in a text, we can create a simple variant of the Rabin–Karp
 * algorithm that uses a Bloom filter or a set data structure to check whether
 * the hash of a given string belongs to a set of hash values of patterns we
 * are looking for
 *
 * #
 * ...
 * To avoid recalculating the hash for each window to compare in $text a
 * rolling hash function is used in which the input is hashed in a window that
 * moves through the input.
 */
namespace sp {
namespace rk {
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
