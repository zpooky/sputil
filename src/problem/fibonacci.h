#ifndef SP_UTIL_PROBLEM_FIBONACCI_H
#define SP_UTIL_PROBLEM_FIBONACCI_H

#include <cstdint>

namespace sp {
std::uint32_t fibonacci(std::uint32_t) noexcept;

namespace dp {
std::uint32_t fibonacci(std::uint32_t) noexcept;
}

}

#endif
