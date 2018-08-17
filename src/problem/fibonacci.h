#ifndef SP_UTIL_PROBLEM_FIBONACCI_H
#define SP_UTIL_PROBLEM_FIBONACCI_H

#include <cstdint>

namespace sp {
std::uint32_t fibonacci(std::uint32_t) noexcept;

namespace dp {
namespace memo {
std::uint32_t fibonacci(std::uint32_t) noexcept;
}

namespace tab {
std::uint32_t fibonacci(std::uint32_t) noexcept;
}

}
}

#endif
