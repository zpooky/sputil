#ifndef SP_UTIL_PROBLEM_TOWER_OF_HANOI_H
#define SP_UTIL_PROBLEM_TOWER_OF_HANOI_H

#include <stack/DynamicStack.h>

// https://bradfieldcs.com/algos/recursion/tower-of-hanoi/
namespace sp {
void
tower_of_hanoi(DynamicStack<int> &from, DynamicStack<int> &to) noexcept;
}

#endif
