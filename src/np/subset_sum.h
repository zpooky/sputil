#ifndef SP_UTIL_NP_PROBLEM_H
#define SP_UTIL_NP_PROBLEM_H

#include <collection/Array.h>
// TODO

// https://en.wikipedia.org/wiki/Karp%27s_21_NP-complete_problems
namespace np {
namespace complete {
/*
 * The problem is this: given a set (or multiset) of integers, is there a
 * non-empty subset whose sum is zero? For example, given the set {−7, −3, −2,
 * 5, 8}, the answer is yes because the subset {−3, −2, 5} sums to zero.
 *
 * The problem is NP-complete, meaning roughly that while it is easy to confirm
 * whether a proposed solution is valid, it may inherently be prohibitively
 * difficult to determine in the first place whether any solution exists.
 */

bool
subset_sum(const sp::Array<int> &set) noexcept;
}

//-----
// # napsack problem
// The decision problem form of the knapsack problem (Can a value of at least V
// be achieved without exceeding the weight W?) is NP-complete, thus there is no
// known algorithm both correct and fast (polynomial-time) in all cases.
}
#endif
