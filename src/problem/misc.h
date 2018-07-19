#ifndef SP_UTIL_PROBLEM_MISC_H
#define SP_UTIL_PROBLEM_MISC_H

#include <cstddef>
#include <cstdint>

namespace sp {
/* Given $N numbers $pool, we need to tell
 * the total number of ways we can form a number $N
 * using the sum of the given $pool numbers.
 * Allowing repetitions and different arrangements.
 */
std::size_t
represent(std::uint32_t N, const std::uint32_t *pool, std::size_t len) noexcept;

namespace dp {
std::size_t
represent(std::uint32_t N, const std::uint32_t *pool, std::size_t len) noexcept;
}

/* Given n $dice each with m $faces, numbered from 1 to m, find the number of
 * ways to get sum $N. $N is the summation of values on each face when all the
 * dice are thrown.
 */
std::size_t
dice_throw(std::size_t N, std::size_t faces, std::size_t dice) noexcept;
}

#endif
